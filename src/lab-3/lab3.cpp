#include <cmath>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "../window.h"
#include "lab3.h"
#include <numbers>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

using Mat3 = std::array<std::array<float, 3>, 3>;

constexpr double PI = 3.14159265358979323846;

constexpr double degToRad(double degrees) { return degrees * (PI / 180.0); }

Mat3 TranslationMatrix(float tx, float ty) {
  return {{{1, 0, tx}, {0, 1, ty}, {0, 0, 1}}};
}

Mat3 ScalingMatrix(float sx, float sy) {
  return {{{sx, 0, 0}, {0, sy, 0}, {0, 0, 1}}};
}

Mat3 RotationMatrix(int degrees) {
  float radians = degToRad(degrees);
  float sinValue = std::sin(radians);
  float cosValue = std::cos(radians);
  return {{{cosValue, -sinValue, 0}, {sinValue, cosValue, 0}, {0, 0, 1}}};
}

Rectangle CreateRectangle(float x, float y, float w, float h) {
  Rectangle r;
  r.x = x;
  r.y = y;
  r.width = w;
  r.height = h;
  r.vertices = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}};

  return r;
}

void createMesh(const std::vector<std::pair<int, int>> &points,
                unsigned int &VAO, unsigned int &VBO) {
  std::vector<float> vertices;
  vertices.reserve(points.size() * 2);
  for (const auto &p : points) {
    vertices.push_back(((float)p.first / SCR_WIDTH) * 2.0f - 1.0f);
    vertices.push_back(((float)p.second / SCR_HEIGHT) * 2.0f - 1.0f);
  }
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

std::vector<std::pair<int, int>> MidPointEllipse(int xc, int yc, int rx,
                                                 int ry) {
  std::vector<std::pair<int, int>> points;
  int x = 0, y = ry;
  double rx2 = (double)rx * rx, ry2 = (double)ry * ry;
  double p1 = ry2 - rx2 * ry + 0.25 * rx2;
  points.push_back({xc, yc});
  points.push_back({xc, yc + ry});
  points.push_back({xc, yc - ry});
  points.push_back({xc + rx, yc});
  points.push_back({xc - rx, yc});

  while (ry2 * x < rx2 * y) {
    x += 1;
    if (p1 < 0) {
      p1 += 2 * ry2 * x + ry2;
    } else {
      y -= 1;
      p1 += 2 * ry2 * x - 2 * rx2 * y + ry2;
    }
    points.push_back({xc + x, yc + y});
    points.push_back({xc - x, yc + y});
    points.push_back({xc - x, yc - y});
    points.push_back({xc + x, yc - y});
  }

  double p2 = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;

  while (y != 0) {
    y -= 1;
    if (p2 < 0) {
      x += 1;
      p2 += 2 * ry2 * x - 2 * rx2 * y + rx2;
    } else {
      p2 += -2 * rx2 * y + rx2;
    }
    points.push_back({xc + x, yc + y});
    points.push_back({xc - x, yc + y});
    points.push_back({xc - x, yc - y});
    points.push_back({xc + x, yc - y});
  }
  return points;
}

std::pair<float, float> ApplyTransformation(Mat3 &m,
                                            std::pair<float, float> p) {
  float vec[3] = {p.first, p.second, 1.0f};
  float out[3] = {0, 0, 0};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      out[i] += m[i][j] * vec[j];
    }
  }
  return {out[0], out[1]};
}

Rectangle TranslateRectangle(Rectangle &rect, float dx, float dy) {
  Mat3 matrix = TranslationMatrix(dx, dy);
  Rectangle out = rect;
  out.vertices.clear();
  for (auto &vectices : rect.vertices) {
    out.vertices.push_back(ApplyTransformation(matrix, vectices));
  }
  return out;
}

Rectangle RotateRectangle(Rectangle &rect, float degree) {
  Mat3 rot = RotationMatrix(degree);

  float cx = rect.x + rect.width / 2.0f;
  float cy = rect.y + rect.height / 2.0f;

  // Mat3 toOrigin = TranslationMatrix(-cx, -cy);
  // Mat3 backToPlace = TranslationMatrix(cx, cy);

  Mat3 toOrigin = TranslationMatrix(-rect.x, -rect.y);
  Mat3 backToPlace = TranslationMatrix(rect.x, rect.y);

  Rectangle out = rect;
  out.vertices.clear();
  for (auto &v : rect.vertices) {
    auto centered = ApplyTransformation(toOrigin, v);
    auto rotated = ApplyTransformation(rot, centered);
    auto final_ = ApplyTransformation(backToPlace, rotated);
    out.vertices.push_back(final_);
  }
  return out;
}

Rectangle SclaeRectangle(Rectangle &x, float sx, float sy) {}

void Transformation(GLFWwindow *window, unsigned int shaderProgram) {

  // auto linePoints = MidPointEllipse(300, 300, 100, 150);
  // createMesh(linePoints, VAO, VBO);

  unsigned int VAO, VBO;
  Rectangle rect = CreateRectangle(100, 100, 150, 80);
  // Rectangle moved = TranslateRectangle(rect, 10, -20);
  Rectangle moved = RotateRectangle(rect, 45);

  std::vector<std::pair<int, int>> linePoints;
  for (auto &v : rect.vertices)
    linePoints.push_back({(int)v.first, (int)v.second});
  for (auto &v : moved.vertices)
    linePoints.push_back({(int)v.first, (int)v.second});
  createMesh(linePoints, VAO, VBO);
  glPointSize(3.0f);

  int colorLoc = glGetUniformLocation(shaderProgram, "uColor");

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // glDrawArrays(GL_POINTS, 0, linePoints.size());
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // white
    glDrawArrays(GL_LINE_LOOP, 0, 4);        // rect
                                             //
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // red
    glDrawArrays(GL_LINE_LOOP, 4, 4);        // moved

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}
