#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void createMesh(const std::vector<std::pair<int, int>> &points,
                unsigned int &VAO, unsigned int &VBO) {
  std::vector<float> vertices;
  vertices.reserve(points.size() * 2);
  for (const auto &p : points) {
    vertices.push_back(((float)p.first / SCR_WIDTH) * 2.0f - 1.0f);
    vertices.push_back(((float)p.second / SCR_HEIGHT) * 2.0f - 1.0f);
  }
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}
