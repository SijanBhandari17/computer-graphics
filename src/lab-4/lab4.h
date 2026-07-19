#ifndef LAB4_H
#define LAB4_H
#include <GLFW/glfw3.h>

struct Line {
  double x1;
  double y1;
  double x2;
  double y2;
  int codel, coder;
};

void ClipLine(GLFWwindow *window, unsigned int shaderProgram);

#endif
