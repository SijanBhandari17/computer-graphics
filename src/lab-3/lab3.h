#ifndef LAB3_H
#define LAB3_H
#include <vector>
#include <GLFW/glfw3.h>
#include <array>

std::vector<std::pair<int, int>> MidPointEllipse(int xc, int yc, int rx,
                                                 int ry);

void Transformation(GLFWwindow *window, unsigned int shaderProgram);

struct Rectangle {
  float x, y;
  float width, height;
  std::array<float, 12> vertices;
};

#endif
