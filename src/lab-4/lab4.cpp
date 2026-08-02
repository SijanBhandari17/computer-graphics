#include <algorithm>
#include <array>
#include <glad/glad.h>
#include "lab4.h"
#include <vector>
#include "../mesh.h"
#include "../window.h"
#include <GLFW/glfw3.h>

const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

const int WINDOW_X_MIN = 200;
const int WINDOW_Y_MIN = 200;
const int WINDOW_X_MAX = 700;
const int WINDOW_Y_MAX = 400;

int computeCode(double x, double y) {
  int code = INSIDE;

  if (x < WINDOW_X_MIN)
    code |= LEFT;
  else if (x > WINDOW_X_MAX)
    code |= RIGHT;

  if (y < WINDOW_Y_MIN)
    code |= BOTTOM;
  else if (y > WINDOW_Y_MAX)
    code |= TOP;

  return code;
}

bool CohenSutherlandClipLines(Line &l) {
  double x1 = l.x1, y1 = l.y1, x2 = l.x2, y2 = l.y2;
  int code1 = computeCode(x1, y1);
  int code2 = computeCode(x2, y2);
  bool accept = false;
  while (true) {
    if ((code1 | code2) == 0) {
      accept = true;
      break;
    } else if ((code1 & code2) != 0) {
      break;
    } else {
      double x, y;
      int codeOut = code1 != 0 ? code1 : code2;
      double slope = (y2 - y1) / (x2 - x1);
      if (codeOut & TOP) {
        x = x1 + (WINDOW_Y_MAX - y1) / slope;
        y = WINDOW_Y_MAX;
      } else if (codeOut & BOTTOM) {
        x = x1 + (WINDOW_Y_MIN - y1) / slope;
        y = WINDOW_Y_MIN;
      } else if (codeOut & RIGHT) {
        y = y1 + slope * (WINDOW_X_MAX - x1);
        x = WINDOW_X_MAX;
      } else {
        y = y1 + slope * (WINDOW_X_MIN - x1);
        x = WINDOW_X_MIN;
      }
      if (codeOut == code1) {
        x1 = x;
        y1 = y;
        code1 = computeCode(x1, y1);
      } else {
        x2 = x;
        y2 = y;
        code2 = computeCode(x2, y2);
      }
    }
  }
  if (accept) {
    l.x1 = x1;
    l.y1 = y1;
    l.x2 = x2;
    l.y2 = y2;
  }
  return accept;
}

bool LiangBarskyClipLine(Line &l) {
  double dx = l.x2 - l.x1;
  double dy = l.y2 - l.y1;

  std::array<double, 4> p = {-dx, dx, -dy, dy};
  std::array<double, 4> q = {l.x1 - WINDOW_X_MIN, WINDOW_X_MAX - l.x1,
                             l.y1 - WINDOW_Y_MIN, WINDOW_Y_MAX - l.y1};

  double t1 = 0, t2 = 1;

  for (int i = 0; i < 4; i++) {
    if (p[i] == 0 && q[i] < 0)
      return false;
    if (p[i] < 0) {
      double r = q[i] / p[i];
      t1 = std::max(t1, r);
    }
    if (p[i] > 0) {
      double r = q[i] / p[i];
      t2 = std::min(t2, r);
    }
    if (t1 > t2)
      return false;
  }

  l.x2 = l.x1 + t2 * dx;
  l.y2 = l.y1 + t2 * dy;
  l.x1 = l.x1 + t1 * dx;
  l.y1 = l.y1 + t1 * dy;

  return true;
}

enum class Edge { LEFT, RIGHT, BOTTOM, TOP };

bool insideEdge(const std::pair<double, double> &p, Edge edge) {
  switch (edge) {
  case Edge::LEFT:
    return p.first >= WINDOW_X_MIN;
  case Edge::RIGHT:
    return p.first <= WINDOW_X_MAX;
  case Edge::BOTTOM:
    return p.second >= WINDOW_Y_MIN;
  case Edge::TOP:
    return p.second <= WINDOW_Y_MAX;
  }
  return false;
}

std::pair<double, double> intersectEdge(const std::pair<double, double> &p1,
                                        const std::pair<double, double> &p2,
                                        Edge edge) {
  double x1 = p1.first, y1 = p1.second;
  double x2 = p2.first, y2 = p2.second;
  double x, y;

  switch (edge) {
  case Edge::LEFT:
    x = WINDOW_X_MIN;
    y = y1 + (y2 - y1) * (WINDOW_X_MIN - x1) / (x2 - x1);
    break;
  case Edge::RIGHT:
    x = WINDOW_X_MAX;
    y = y1 + (y2 - y1) * (WINDOW_X_MAX - x1) / (x2 - x1);
    break;
  case Edge::BOTTOM:
    y = WINDOW_Y_MIN;
    x = x1 + (x2 - x1) * (WINDOW_Y_MIN - y1) / (y2 - y1);
    break;
  case Edge::TOP:
    y = WINDOW_Y_MAX;
    x = x1 + (x2 - x1) * (WINDOW_Y_MAX - y1) / (y2 - y1);
    break;
  }
  return {x, y};
};

std::vector<std::pair<double, double>>
clipAgainstEdge(const std::vector<std::pair<double, double>> &polygon,
                Edge edge) {
  std::vector<std::pair<double, double>> output;
  if (polygon.empty())
    return output;

  size_t n = polygon.size();
  for (size_t i = 0; i < n; i++) {
    const auto &current = polygon[i];
    const auto &prev = polygon[(i + n - 1) % n];

    bool currentInside = insideEdge(current, edge);
    bool prevInside = insideEdge(prev, edge);

    if (currentInside) {
      if (!prevInside)
        output.push_back(intersectEdge(prev, current, edge));
      output.push_back(current);
    } else if (prevInside) {
      output.push_back(intersectEdge(prev, current, edge));
    }
  }
  return output;
}

std::vector<std::pair<double, double>>
SutherlandHodgemanClip(const std::vector<std::pair<double, double>> &polygon) {
  std::vector<std::pair<double, double>> output = polygon;
  output = clipAgainstEdge(output, Edge::LEFT);
  output = clipAgainstEdge(output, Edge::RIGHT);
  output = clipAgainstEdge(output, Edge::BOTTOM);
  output = clipAgainstEdge(output, Edge::TOP);
  return output;
}

void ClipPolygon(GLFWwindow *window, unsigned int shaderProgram) {
  std::vector<std::pair<double, double>> subject = {
      {100, 150}, {350, 100}, {600, 450}, {450, 500}, {150, 350}};

  std::vector<std::pair<int, int>> originalPoints;
  for (auto &p : subject)
    originalPoints.push_back({(int)p.first, (int)p.second});

  auto clipped = SutherlandHodgemanClip(subject);

  std::vector<std::pair<int, int>> clippedPoints;
  for (auto &p : clipped)
    clippedPoints.push_back({(int)p.first, (int)p.second});

  std::vector<std::pair<int, int>> windowPoints = {
      {WINDOW_X_MIN, WINDOW_Y_MIN},
      {WINDOW_X_MAX, WINDOW_Y_MIN},
      {WINDOW_X_MAX, WINDOW_Y_MAX},
      {WINDOW_X_MIN, WINDOW_Y_MAX}};

  unsigned windowVAO, windowVBO, subjectVAO, subjectVBO, clippedVAO, clippedVBO;
  createMesh(windowPoints, windowVAO, windowVBO);
  createMesh(originalPoints, subjectVAO, subjectVBO);
  createMesh(clippedPoints, clippedVAO, clippedVBO);

  int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);

    glBindVertexArray(windowVAO);
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // white - clip window
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(subjectVAO);
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // red - original polygon
    glDrawArrays(GL_LINE_LOOP, 0, originalPoints.size());

    if (!clippedPoints.empty()) {
      glBindVertexArray(clippedVAO);
      glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // green - clipped polygon
      glDrawArrays(GL_LINE_LOOP, 0, clippedPoints.size());
    }

    glBindVertexArray(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void ClipLine(GLFWwindow *window, unsigned int shaderProgram) {
  Line l = {500, 500, 300, 300};

  std::vector<std::pair<int, int>> originalLines = {{l.x1, l.y1}, {l.x2, l.y2}};

  bool accept = LiangBarskyClipLine(l);
  std::vector<std::pair<int, int>> windowPoints = {
      {WINDOW_X_MIN, WINDOW_Y_MIN},
      {WINDOW_X_MAX, WINDOW_Y_MIN},
      {WINDOW_X_MAX, WINDOW_Y_MAX},
      {WINDOW_X_MIN, WINDOW_Y_MAX}};

  unsigned windowVAO, windowVBO, lineVAO, lineVBO, originalVAO, originalVBO;
  createMesh(windowPoints, windowVAO, windowVBO);

  createMesh(originalLines, originalVAO, originalVBO);

  std::vector<std::pair<int, int>> linePoints = {};

  if (accept) {
    linePoints.push_back({l.x1, l.y1});
    linePoints.push_back({l.x2, l.y2});
  }

  createMesh(linePoints, lineVAO, lineVBO);

  int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glBindVertexArray(windowVAO);
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // white
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(originalVAO);
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // red
    glDrawArrays(GL_LINES, 0, originalLines.size());

    glBindVertexArray(lineVAO);
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // white
    glDrawArrays(GL_LINES, 0, linePoints.size());

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
