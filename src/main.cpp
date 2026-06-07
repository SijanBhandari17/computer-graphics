#include "shaders.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lab-1/lab1.h"
#include "window.h"

int main() {

  GLFWwindow *window = createWindow(800, 600, "OpenGL First Name");
  if (!window)
    return -1;

  unsigned int shaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);

  loadAllLetters(window, shaderProgram);

  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}
