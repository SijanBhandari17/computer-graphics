#include <cmath>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "../window.h"
#include "../mesh.h"
#include "lab3.h"
#include <array>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

using Mat3 = std::array<std::array<float, 3>, 3>;

constexpr double PI = 3.14159265358979323846;

constexpr double degToRad(double degrees) { return degrees * (PI / 180.0); }

Rectangle CreateRectangle(float x, float y, float w, float h) {
  Rectangle r;
  r.x = x;
  r.y = y;
  r.width = w;
  r.height = h;
  r.vertices = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}};

  return r;
}

Mat3 MultiplyMatrices(const Mat3 &a, const Mat3 &b) {
  Mat3 result{};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      result[i][j] = 0;
      for (int k = 0; k < 3; k++) {
        result[i][j] += a[i][k] * b[k][j];
      }
    }
  }
  return result;
}

std::vector<std::pair<int, int>> MidPointEllipse(int xc, int yc, int rx,
                                                 int ry) {
  std::vector<std::pair<int, int>> points;
  int x = 0, y = ry;
  double rx2 = (double)rx * rx, ry2 = (double)ry * ry;
  double p1 = ry2 - rx2 * ry + 0.25 * rx2;
  points.push_back({xc, yc});
  points.push_back({xc, yc + ry});
  points.push_back({xc, yc - ry});
  points.push_back({xc + rx, yc});
  points.push_back({xc - rx, yc});

  while (ry2 * x < rx2 * y) {
    x += 1;
    if (p1 < 0) {
      p1 += 2 * ry2 * x + ry2;
    } else {
      y -= 1;
      p1 += 2 * ry2 * x - 2 * rx2 * y + ry2;
    }
    points.push_back({xc + x, yc + y});
    points.push_back({xc - x, yc + y});
    points.push_back({xc - x, yc - y});
    points.push_back({xc + x, yc - y});
  }

  double p2 = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;

  while (y != 0) {
    y -= 1;
    if (p2 < 0) {
      x += 1;
      p2 += 2 * ry2 * x - 2 * rx2 * y + rx2;
    } else {
      p2 += -2 * rx2 * y + rx2;
    }
    points.push_back({xc + x, yc + y});
    points.push_back({xc - x, yc + y});
    points.push_back({xc - x, yc - y});
    points.push_back({xc + x, yc - y});
  }
  return points;
}

std::pair<float, float> ApplyTransformation(Mat3 &m,
                                            std::pair<float, float> p) {
  float vec[3] = {p.first, p.second, 1.0f};
  float out[3] = {0, 0, 0};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      out[i] += m[i][j] * vec[j];
    }
  }
  return {out[0], out[1]};
}

Mat3 TranslationMatrix(float tx, float ty) {
  return {{{1, 0, tx}, {0, 1, ty}, {0, 0, 1}}};
}
Rectangle TranslateRectangle(Rectangle &rect, float dx, float dy) {
  Mat3 matrix = TranslationMatrix(dx, dy);
  Rectangle out = rect;
  out.vertices.clear();
  for (auto &vectices : rect.vertices) {
    out.vertices.push_back(ApplyTransformation(matrix, vectices));
  }
  return out;
}

Mat3 RotationMatrix(int degrees) {
  float radians = degToRad(degrees);
  float sinValue = std::sin(radians);
  float cosValue = std::cos(radians);
  return {{{cosValue, -sinValue, 0}, {sinValue, cosValue, 0}, {0, 0, 1}}};
}
Rectangle RotateRectangle(Rectangle &rect, float degree) {
  Mat3 rot = RotationMatrix(degree);

  // float cx = rect.x + rect.width / 2.0f;
  // float cy = rect.y + rect.height / 2.0f;

  // Mat3 toOrigin = TranslationMatrix(-cx, -cy);
  // Mat3 backToPlace = TranslationMatrix(cx, cy);

  Mat3 toOrigin = TranslationMatrix(-rect.x, -rect.y);
  Mat3 backToPlace = TranslationMatrix(rect.x, rect.y);

  Rectangle out = rect;
  out.vertices.clear();
  for (auto &v : rect.vertices) {
    auto centered = ApplyTransformation(toOrigin, v);
    auto rotated = ApplyTransformation(rot, centered);
    auto final_ = ApplyTransformation(backToPlace, rotated);
    out.vertices.push_back(final_);
  }
  return out;
}

Mat3 ScalingMatrix(float sx, float sy) {
  return {{{sx, 0, 0}, {0, sy, 0}, {0, 0, 1}}};
}
Rectangle ScaleRectangle(Rectangle &rect, float sx, float sy) {

  Mat3 rot = ScalingMatrix(sx, sy);
  Mat3 toOrigin = TranslationMatrix(-rect.x, -rect.y);
  Mat3 backToPlace = TranslationMatrix(rect.x, rect.y);

  Rectangle out = rect;
  out.vertices.clear();
  for (auto &v : rect.vertices) {
    auto centered = ApplyTransformation(toOrigin, v);
    auto rotated = ApplyTransformation(rot, centered);
    auto final_ = ApplyTransformation(backToPlace, rotated);
    out.vertices.push_back(final_);
  }
  return out;
}

Mat3 ReflectXMatrix() { return {{{1, 0, 0}, {0, -1, 0}, {0, 0, 1}}}; }
Mat3 ReflectYMatrix() { return {{{-1, 0, 0}, {0, 1, 0}, {0, 0, 1}}}; }
Mat3 ReflectOriginMatrix() { return {{{-1, 0, 0}, {0, -1, 0}, {0, 0, 1}}}; }
Mat3 ReflectYEqualsXMatrix() { return {{{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}}; }

Rectangle ReflectRectangle(Rectangle &rect) {

  Mat3 rot = ReflectOriginMatrix();

  Mat3 toOrigin = TranslationMatrix(-rect.x, -rect.y);
  Mat3 backToPlace = TranslationMatrix(rect.x, rect.y);

  Rectangle out = rect;
  out.vertices.clear();
  for (auto &v : rect.vertices) {
    auto centered = ApplyTransformation(toOrigin, v);
    auto rotated = ApplyTransformation(rot, centered);
    auto final_ = ApplyTransformation(backToPlace, rotated);
    out.vertices.push_back(final_);
  }
  return out;
}

Mat3 ShearMatrix(float shx, float shy) {
  return {{{1, shx, 0}, {shy, 1, 0}, {0, 0, 1}}};
}
Rectangle ShearRectangle(Rectangle &rect, float shx, float shy) {

  Mat3 rot = ShearMatrix(shx, shy);

  Mat3 toOrigin = TranslationMatrix(-rect.x, -rect.y);
  Mat3 backToPlace = TranslationMatrix(rect.x, rect.y);

  Rectangle out = rect;
  out.vertices.clear();
  for (auto &v : rect.vertices) {
    auto centered = ApplyTransformation(toOrigin, v);
    auto rotated = ApplyTransformation(rot, centered);
    auto final_ = ApplyTransformation(backToPlace, rotated);
    out.vertices.push_back(final_);
  }
  return out;
}

enum class TransformType { TRANSLATE, ROTATE, SCALE, SHEAR };

struct TransformOp {
  TransformType type;
  float p1 = 0.0f;
  float p2 = 0.0f;
};

Mat3 BuildOpMatrix(const TransformOp &op, float cx, float cy) {
  Mat3 toOrigin = TranslationMatrix(-cx, -cy);
  Mat3 backToPlace = TranslationMatrix(cx, cy);

  switch (op.type) {
  case TransformType::TRANSLATE:
    return TranslationMatrix(op.p1, op.p2);
  case TransformType::ROTATE:
    return MultiplyMatrices(backToPlace,
                            MultiplyMatrices(RotationMatrix(op.p1), toOrigin));
  case TransformType::SCALE:
    return MultiplyMatrices(
        backToPlace, MultiplyMatrices(ScalingMatrix(op.p1, op.p2), toOrigin));
  case TransformType::SHEAR:
    return MultiplyMatrices(
        backToPlace, MultiplyMatrices(ShearMatrix(op.p1, op.p2), toOrigin));
  }
  return TranslationMatrix(0, 0);
}

Rectangle CompositeTransform(Rectangle &rect,
                             const std::vector<TransformOp> &ops) {
  float cx = rect.x;
  float cy = rect.y;

  Mat3 composite = TranslationMatrix(0, 0);

  for (const auto &op : ops) {
    Mat3 opMat = BuildOpMatrix(op, cx, cy);
    composite = MultiplyMatrices(opMat, composite);
  }

  Rectangle out = rect;
  out.vertices.clear();
  for (auto &v : rect.vertices)
    out.vertices.push_back(ApplyTransformation(composite, v));
  return out;
}

std::vector<TransformOp> GetUserTransformSequence() {
  std::vector<TransformOp> ops;
  int n;
  std::cout << "How many transformations to chain? ";
  std::cin >> n;

  for (int i = 0; i < n; ++i) {
    std::cout << "\nTransform " << i + 1
              << ": 1) Translate  2) Rotate  3) Scale  4) Shear\nChoice: ";
    int choice;
    std::cin >> choice;

    TransformOp op;
    switch (choice) {
    case 1:
      op.type = TransformType::TRANSLATE;
      std::cout << "  dx dy: ";
      std::cin >> op.p1 >> op.p2;
      break;
    case 2:
      op.type = TransformType::ROTATE;
      std::cout << "  angle (deg): ";
      std::cin >> op.p1;
      break;
    case 3:
      op.type = TransformType::SCALE;
      std::cout << "  sx sy: ";
      std::cin >> op.p1 >> op.p2;
      break;
    case 4:
      op.type = TransformType::SHEAR;
      std::cout << "  shx shy: ";
      std::cin >> op.p1 >> op.p2;
      break;
    default:
      std::cout << "  invalid, using no-op translate(0,0)\n";
      op.type = TransformType::TRANSLATE;
      break;
    }
    ops.push_back(op);
  }
  return ops;
}

void Transformation(GLFWwindow *window, unsigned int shaderProgram) {

  // auto linePoints = MidPointEllipse(300, 300, 100, 150);
  // createMesh(linePoints, VAO, VBO);

  unsigned int VAO, VBO;
  Rectangle rect = CreateRectangle(300, 200, 150, 80);
  // Rectangle moved = TranslateRectangle(rect, 10, -20);
  // Rectangle moved = RotateRectangle(rect, 45);
  // Rectangle moved = ShearRectangle(rect, 0, 1);
  // Rectangle moved =
  //     CompositeTransform(rect, 50, 30, 45, 1.2f, 1.2f, 0.2f, 0.0f);
  // Rectangle moved = ScaleRectangle(rect, 2, 2);

  // Rectangle moved = ReflectRectangle(rect);
  std::vector<TransformOp> ops = GetUserTransformSequence();
  Rectangle moved = CompositeTransform(rect, ops);
  std::vector<std::pair<int, int>> linePoints;
  for (auto &v : rect.vertices)
    linePoints.push_back({(int)v.first, (int)v.second});
  for (auto &v : moved.vertices)
    linePoints.push_back({(int)v.first, (int)v.second});

  createMesh(linePoints, VAO, VBO);
  glPointSize(3.0f);

  int colorLoc = glGetUniformLocation(shaderProgram, "uColor");

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // glDrawArrays(GL_POINTS, 0, linePoints.size());
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // white
    glDrawArrays(GL_LINE_LOOP, 0, 4);        // rect
                                             //
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // red
    glDrawArrays(GL_LINE_LOOP, 4, 4);        // moved

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}
