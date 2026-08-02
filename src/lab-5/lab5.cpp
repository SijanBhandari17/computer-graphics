#include <glad/glad.h>
#include "../mesh.h"
#include <GLFW/glfw3.h>
#include "lab5.h"
#include "../window.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// clang-format off
static float cubeVertices[] = {
    // back face
    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f,
    // front face
    0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f,
    // bottom face
    0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    // top face
    0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
    // left face
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
    // right face
    1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
};
// clang-format on

void Cube::Init() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

  // matches layout(location = 0) in vec3 aPos; in your vertex shader
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void Cube::Draw(unsigned int shaderProgram) {
  glUseProgram(shaderProgram);
  // per-face colors need either per-vertex color attributes or multiple draw calls with uColor set
  // — for now, set one color via the uniform your fragment shader already expects
  int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
  glm::vec3 faceColors[6] = {
      {1.0f, 0.0f, 0.0f},  // back  - red
      {0.0f, 1.0f, 0.0f},  // front - green
      {0.0f, 0.0f, 1.0f},  // bottom- blue
      {1.0f, 1.0f, 0.0f},  // top   - yellow
      {1.0f, 0.0f, 1.0f},  // left  - magenta
      {0.0f, 1.0f, 1.0f},  // right - cyan
  };

  glBindVertexArray(VAO);
  for (int i = 0; i < 6; i++) {
    glUniform3f(colorLoc, faceColors[i].x, faceColors[i].y, faceColors[i].z);
    glDrawArrays(GL_TRIANGLES, i * 6, 6);  // 6 vertices per face
  }
  glBindVertexArray(0);
}

void DrawCube(GLFWwindow* window, unsigned int shaderProgram) {
  Cube cube;
  cube.Init();

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glm::mat4 model =
        glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -4.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 mvp = proj * view * model;

    int mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    cube.Draw(shaderProgram);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
