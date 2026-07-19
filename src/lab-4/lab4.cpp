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
      } else { // LEFT
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

void ClipLine(GLFWwindow *window, unsigned int shaderProgram) {
  Line l1 = {50, 50, 300, 300};

  std::vector<std::pair<int, int>> originalLines = {{l1.x1, l1.y1},
                                                    {l1.x2, l1.y2}};

  bool accept = CohenSutherlandClipLines(l1);
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
    linePoints.push_back({l1.x1, l1.y1});
    linePoints.push_back({l1.x2, l1.y2});
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
