#ifndef LAB2_H
#define LAB2_H
#include <vector>
#include <GLFW/glfw3.h>

std::vector<std::pair<int, int>> DDA(int x1, int x2, int y1, int y2);
std::vector<std::pair<int, int>> BressenhamLineDrawing(int x1, int x2, int y1,
                                                       int y2);
void createMesh(const std::vector<std::pair<int, int>> &points,
                unsigned int &VAO, unsigned int &VBO);
void drawShapes(GLFWwindow *window, unsigned int shaderProgram);

#endif
