#include "shaders.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lab-2/lab2.h"
#include "lab-3/lab3.h"
#include "window.h"

int main() {

  GLFWwindow *window = createWindow(800, 600, "OpenGL First Name");
  if (!window)
    return -1;

  unsigned int shaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);

  Transformation(window, shaderProgram);

  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}
