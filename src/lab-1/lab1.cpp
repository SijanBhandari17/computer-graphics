#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "lab1.h"
#include "../window.h"

const std::vector<std::vector<float>> letterS = {
    // top bar
    {-0.9f, 0.6f, 0.0f, -0.9f, 0.5f, 0.0f, -0.5f, 0.6f, 0.0f, -0.5f, 0.5f,
     0.0f},
    // left-top stroke
    {-0.9f, 0.5f, 0.0f, -0.9f, 0.0f, 0.0f, -0.8f, 0.5f, 0.0f, -0.8f, 0.0f,
     0.0f},
    // middle bar
    {-0.9f, 0.0f, 0.0f, -0.9f, 0.1f, 0.0f, -0.5f, 0.0f, 0.0f, -0.5f, 0.1f,
     0.0f},
    // right-bottom stroke
    {-0.5f, 0.0f, 0.0f, -0.5f, -0.5f, 0.0f, -0.6f, 0.0f, 0.0f, -0.6f, -0.5f,
     0.0f},
    // bottom bar
    {-0.9f, -0.5f, 0.0f, -0.9f, -0.4f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f, -0.4f,
     0.0f},
};

const std::vector<std::vector<float>> letterI = {
    // top bar
    {-0.4f, 0.6f, 0.0f, -0.4f, 0.5f, 0.0f, 0.1f, 0.6f, 0.0f, 0.1f, 0.5f, 0.0f},
    // middle bar
    {-0.2f, 0.5f, 0.0f, -0.1f, 0.5f, 0.0f, -0.2f, -0.4f, 0.0f, -0.1f, -0.4f,
     0.0f},
    // bottom bar
    {-0.4f, -0.4f, 0.0f, -0.4f, -0.5f, 0.0f, 0.1f, -0.4f, 0.0f, 0.1f, -0.5f,
     0.0f}};

const std::vector<std::vector<float>> letterJ = {
    // top bar
    {0.2f, 0.6f, 0.0f, 0.2f, 0.5f, 0.0f, 0.7f, 0.6f, 0.0f, 0.7f, 0.5f, 0.0f},
    // middle bar
    {0.4f, 0.5f, 0.0f, 0.4f, -0.4f, 0.0f, 0.5f, 0.5f, 0.0f, 0.5f, -0.4f, 0.0f},
    // bottom bar
    {0.2f, -0.4f, 0.0f, 0.2f, -0.5f, 0.0f, 0.5f, -0.4f, 0.0f, 0.5f, -0.5f,
     0.0f}};

const std::vector<std::vector<float>> letterA = {
    // '/'
    {0.85f, 0.6f, 0.0f, 0.75f, -0.5f, 0.0f, 0.95f, 0.6f, 0.0f, 0.85f, -0.5f,
     0.0f},
    //  '\'
    {0.95f, 0.6f, 0.0f, 1.05f, -0.5f, 0.0f, 1.05f, 0.6f, 0.0f, 1.15f, -0.5f,
     0.0f},
    //  middle bar
    {0.9f, 0.05f, 0.0f, 0.9f, -0.05f, 0.0f, 1.0f, 0.05f, 0.0f, 1.0f, -0.05f,
     0.0f}};

const std::vector<std::vector<float>> letterN = {
    // '|'
    {1.25f, 0.6f, 0.0f, 1.25f, -0.5f, 0.0f, 1.35f, 0.6f, 0.0f, 1.35f, -0.5f,
     0.0f},
    //  '\'
    {1.35f, 0.6f, 0.0f, 1.45f, -0.5f, 0.0f, 1.45f, 0.6f, 0.0f, 1.55f, -0.5f,
     0.0f},
    //  '|'
    {1.55f, 0.6f, 0.0f, 1.55f, -0.5f, 0.0f, 1.65f, 0.6f, 0.0f, 1.65f, -0.5f,
     0.0f}};

Mesh createMesh(const std::vector<float> &vertices, int floatsPerVertex) {
  Mesh mesh;
  mesh.vertexCount = vertices.size() / floatsPerVertex;

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);

  glBindVertexArray(mesh.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE,
                        floatsPerVertex * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  return mesh;
}

void drawMesh(const Mesh &mesh) {
  glBindVertexArray(mesh.VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertexCount);
}

void deleteMesh(Mesh &mesh) {
  glDeleteVertexArrays(1, &mesh.VAO);
  glDeleteBuffers(1, &mesh.VBO);
}

std::vector<float> transform(std::vector<float> verts, float scale,
                             float offsetX, float offsetY) {
  for (int i = 0; i < verts.size(); i += 3) {
    verts[i] = verts[i] * scale + offsetX;
    verts[i + 1] = verts[i + 1] * scale + offsetY;
  }
  return verts;
}

void loadLetter(std::vector<Mesh> &meshes,
                const std::vector<std::vector<float>> &letter, float scale,
                float offsetX, float offsetY) {
  for (auto &verts : letter)
    meshes.push_back(createMesh(transform(verts, scale, offsetX, offsetY), 3));
}

void loadAllLetters(GLFWwindow *window, unsigned int shaderProgram) {

  std::vector<Mesh> meshes;
  float scale = 0.4f;
  float gap = 0.0f;
  loadLetter(meshes, letterS, scale, gap * 0, 0.0f);
  loadLetter(meshes, letterI, scale, gap * 1, 0.0f);
  loadLetter(meshes, letterJ, scale, gap * 2, 0.0f);
  loadLetter(meshes, letterA, scale, gap * 3, 0.0f);
  loadLetter(meshes, letterN, scale, gap * 4, 0.0f);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    for (auto &m : meshes)
      drawMesh(m);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (auto &m : meshes)
    deleteMesh(m);
}
