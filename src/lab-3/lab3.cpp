#include <cmath>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "../window.h"
#include "lab3.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Rectangle CreateRectangle(float x, float y, float w, float h) {
  Rectangle r;
  r.x = x;
  r.y = y;
  r.width = w;
  r.height = h;

  r.vertices = {x, y, x + w, y,     x + w, y + h,

                x, y, x + w, y + h, x,     y + h};

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

void translation(int tx, int ty, std::vector<float> rect) {}

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

void Transformation(GLFWwindow *window, unsigned int shaderProgram) {

  auto linePoints = MidPointEllipse(300, 300, 100, 150);
  unsigned int VAO, VBO;
  createMesh(linePoints, VAO, VBO);

  glPointSize(3.0f);
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, linePoints.size());
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}
