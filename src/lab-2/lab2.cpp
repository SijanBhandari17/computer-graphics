#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <vector>
#include "lab2.h"
#include "../window.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 700;

std::vector<std::pair<int, int>> DDA(int x1, int x2, int y1, int y2) {
  std::vector<std::pair<int, int>> points;
  int dx = x2 - x1;
  int dy = y2 - y1;
  int steps = std::max(abs(dx), abs(dy));
  float xInc = dx / (float)steps;
  float yInc = dy / (float)steps;
  float x = x1;
  float y = y1;
  points.push_back({x1, y1});
  for (int i = 0; i <= steps; i++) {
    std::cout << "Plotting point: (" << std::round(x) << ", " << round(y)
              << ")\n";
    x += xInc;
    y += yInc;
    points.push_back({std::round(x), std::round(y)});
  }
  return points;
}

void createMesh(const std::vector<std::pair<int, int>> &points,
                unsigned int &VAO, unsigned int &VBO) {
  std::vector<float> vertices;
  for (const auto &[x, y] : points) {
    vertices.push_back(((float)x / SCR_WIDTH) * 2.0f - 1.0f);
    vertices.push_back(((float)y / SCR_HEIGHT) * 2.0f - 1.0f);
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

void drawShapes(GLFWwindow *window, unsigned int shaderProgram) {
  std::vector<std::pair<int, int>> linePoints = DDA(100, 300, 200, 500);

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
