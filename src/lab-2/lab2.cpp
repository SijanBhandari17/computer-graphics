#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <vector>
#include "lab2.h"

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

void drawShapes(GLFWwindow *window, unsigned int shaderProgram) {

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    for (auto &m : meshes)
      drawMesh(m);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (auto &m : meshes)
    deleteMesh(m);
}
