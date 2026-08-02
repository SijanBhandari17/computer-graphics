#ifndef LAB5_H
#define LAB5_H
#include <GLFW/glfw3.h>

// mesh.h — add this struct/class or free functions
struct Cube {
  unsigned int VAO, VBO;
  void Init();
  void Draw(unsigned int shaderProgram);
};

void DrawCube(GLFWwindow* window, unsigned int shaderProgram);

#endif
