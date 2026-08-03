#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lab5.h"
#include "../window.h"
#include <glm/ext/matrix_transform.hpp>
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

enum class TransformMode { Translate, Rotate, Shear, Scale };
enum class ProjectionMode { Perspective, Orthographic };

TransformMode currentMode = TransformMode::Translate;
ProjectionMode currentProjection = ProjectionMode::Perspective;

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

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currentMode = TransformMode::Translate;
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) currentMode = TransformMode::Rotate;
  if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) currentMode = TransformMode::Shear;
  if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) currentMode = TransformMode::Scale;

  // toggle projection mode on key-down only (avoid rapid flicker while held)
  static bool pKeyWasPressed = false;
  bool pKeyIsPressed = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
  if (pKeyIsPressed && !pKeyWasPressed) {
    currentProjection = (currentProjection == ProjectionMode::Perspective)
                            ? ProjectionMode::Orthographic
                            : ProjectionMode::Perspective;
  }
  pKeyWasPressed = pKeyIsPressed;
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

    glm::vec3 cameraPos = glm::vec3(2.0f, 3.0f, 6.0f);
    glm::vec3 cameraTarget = glm::vec3(1.5f, 0.5f, 0.5f);  // shifted right to frame both cubes
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);

    glm::mat4 proj;
    if (currentProjection == ProjectionMode::Perspective) {
      proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    } else {
      float orthoSize = 3.0f;  // half-height of the visible box, in world units
      float aspect = 800.0f / 600.0f;
      proj = glm::ortho(-orthoSize * aspect, orthoSize * aspect,  // left, right
                        -orthoSize, orthoSize,                    // bottom, top
                        0.1f, 100.0f);                            // near, far
    }

    int mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");

    // ---- Cube 1: original, untouched ----
    glm::mat4 originalModel = glm::mat4(1.0f);
    glm::mat4 mvpOriginal = proj * view * originalModel;
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpOriginal));
    cube.Draw(shaderProgram);

    // ---- Cube 2: transformed, offset to the side ----
    glm::mat4 transformed =
        glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));  // push right first

    switch (currentMode) {
      case TransformMode::Translate: {
        float offsetx = 1.0f;
        float offsety = 0.5f;
        float offsetz = 0.5f;
        transformed = glm::translate(transformed, glm::vec3(offsetx, offsety, offsetz));
        break;
      }
      case TransformMode::Rotate: {
        float angle = (float)glfwGetTime();
        transformed = glm::rotate(transformed, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
      }
      case TransformMode::Shear: {
        glm::mat4 shear = glm::mat4(1.0f);
        shear[1][0] = 0.5f;
        transformed = transformed * shear;
        break;
      }
      case TransformMode::Scale: {
        float s = 0.5f;
        transformed = glm::scale(transformed, glm::vec3(s, s, s));
        break;
      }
    }

    glm::mat4 mvpTransformed = proj * view * transformed;
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpTransformed));
    cube.Draw(shaderProgram);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
