#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // white
    }
)";

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow *createWindow(int width, int height, const char *title) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window) {
    std::cerr << "[Window] Failed to create window" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "[GLAD] Failed to initialize" << std::endl;
    return nullptr;
  }

  return window;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

unsigned int compileShader(unsigned int type, const char *source) {
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "[Shader] Compile error ("
              << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "):\n"
              << infoLog << std::endl;
  }
  return shader;
}

unsigned int createShaderProgram(const char *vertSrc, const char *fragSrc) {
  unsigned int vert = compileShader(GL_VERTEX_SHADER, vertSrc);
  unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

  unsigned int program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);

  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cerr << "[Shader] Link error:\n" << infoLog << std::endl;
  }

  glDeleteShader(vert);
  glDeleteShader(frag);
  return program;
}

struct Mesh {
  unsigned int VAO, VBO;
  int vertexCount;
};

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

int main() {
  GLFWwindow *window = createWindow(800, 600, "OpenGL Triangle");
  if (!window)
    return -1;

  unsigned int shaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);

  std::vector<std::vector<float>> letterS = {
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

  std::vector<std::vector<float>> letterI = {
      // top bar
      {-0.4f, 0.6f, 0.0f, -0.4f, 0.5f, 0.0f, 0.1f, 0.6f, 0.0f, 0.1f, 0.5f,
       0.0f},
      // middle bar
      {-0.2f, 0.5f, 0.0f, -0.1f, 0.5f, 0.0f, -0.2f, -0.4f, 0.0f, -0.1f, -0.4f,
       0.0f},
      // bottom bar
      {-0.4f, -0.4f, 0.0f, -0.4f, -0.5f, 0.0f, 0.1f, -0.4f, 0.0f, 0.1f, -0.5f,
       0.0f}};

  std::vector<std::vector<float>> letterJ = {
      // top bar
      {-0.4f, 0.6f, 0.0f, -0.4f, 0.5f, 0.0f, 0.1f, 0.6f, 0.0f, 0.1f, 0.5f,
       0.0f},
      // middle bar
      {-0.2f, 0.5f, 0.0f, -0.1f, 0.5f, 0.0f, -0.2f, -0.4f, 0.0f, -0.1f, -0.4f,
       0.0f},
      // bottom bar
      {-0.4f, -0.4f, 0.0f, -0.4f, -0.5f, 0.0f, 0.1f, -0.4f, 0.0f, 0.1f, -0.5f,
       0.0f}};
  std::vector<Mesh> meshS;
  float scale = 0.4f;

  std::vector<Mesh> meshI;
  for (auto &verts : letterS)
    meshS.push_back(createMesh(transform(verts, scale, 0.0f, 0.0f), 3));

  for (auto &verts : letterI)
    meshI.push_back(createMesh(transform(verts, scale, 0.0f, 0.0f), 3));

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // wireframe

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    for (auto &m : meshS)
      drawMesh(m);

    for (auto &m : meshI)
      drawMesh(m);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (auto &m : meshS)
    deleteMesh(m);

  for (auto &m : meshI)
    deleteMesh(m);

  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}
