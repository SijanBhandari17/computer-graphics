#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef WINDOW_H
#define WINDOW_H

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
GLFWwindow *createWindow(int width, int height, const char *title);

#endif
