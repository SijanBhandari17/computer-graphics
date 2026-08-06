#include <glad/glad.h>
#include "lab-5/lab5.h"
#include "lab-4/lab4.h"
#include "shaders.h"
#include <GLFW/glfw3.h>
#include "window.h"

int main() {
  GLFWwindow *window = createWindow(800, 600, "OpenGL First Name");
  if (!window) return -1;

  unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

  DrawCube(window, shaderProgram);
  // ClipLine(window, shaderProgram);
  // ClipPolygon(window, shaderProgram);
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}
