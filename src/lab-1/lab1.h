#ifndef LAB1_H
#define LAB1_H
#include <vector>
#include <GLFW/glfw3.h>
struct Mesh {
  unsigned int VAO, VBO;
  int vertexCount;
};

Mesh createMesh(const std::vector<float> &vertices, int floatsPerVertex);

void drawMesh(const Mesh &mesh);

void deleteMesh(Mesh &mesh);

std::vector<float> transform(std::vector<float> verts, float scale,
                             float offsetX, float offsetY);

extern const std::vector<std::vector<float>> letterS;
extern const std::vector<std::vector<float>> letterI;
extern const std::vector<std::vector<float>> letterJ;
extern const std::vector<std::vector<float>> letterA;
extern const std::vector<std::vector<float>> letterN;

void loadLetter(std::vector<Mesh> &meshes,
                const std::vector<std::vector<float>> &letter, float scale,
                float offsetX, float offsetY);

void loadAllLetters(GLFWwindow *window, unsigned int shaderProgram);

void processInput(GLFWwindow *window);
#endif
