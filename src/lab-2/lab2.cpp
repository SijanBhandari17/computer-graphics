#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <numeric>
#include <vector>
#include <iostream>
#include <cmath>
#include <vector>
#include "lab2.h"
#include "../window.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 700;

void createMesh(const std::vector<std::pair<int, int>> &points,
                unsigned int &VAO, unsigned int &VBO) {
  std::vector<float> vertices;
  for (const auto &[x, y] : points) {
    vertices.push_back(((float)x / SCR_WIDTH) * 2.0f - 1.0f);
    vertices.push_back(((float)y / SCR_WIDTH) * 2.0f - 1.0f);
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

std::vector<std::pair<int, int>> DDA(int x1, int y1, int x2, int y2) {
  std::vector<std::pair<int, int>> points;
  int dx = x2 - x1;
  int dy = y2 - y1;
  int steps = std::max(abs(dx), abs(dy));
  float xInc = dx / (float)steps;
  float yInc = dy / (float)steps;
  float x = x1;
  float y = y1;
  points.push_back({x1, y1});
  for (int i = 0; i <= steps; i++) {
    std::cout << "Plotting point: (" << std::round(x) << ", " << round(y)
              << ")\n";
    x += xInc;
    y += yInc;
    points.push_back({std::round(x), std::round(y)});
  }
  return points;
}

std::vector<std::pair<int, int>> BressenhamLineDrawing(int x1, int y1, int x2,
                                                       int y2) {
  std::vector<std::pair<int, int>> points;
  int del_x = x2 - x1;
  int del_y = y2 - y1;
  int x = x1, y = y1;
  points.push_back({x, y});

  int sx = (del_x > 0) ? 1 : -1;
  int sy = (del_y > 0) ? 1 : -1;
  int adx = abs(del_x);
  int ady = abs(del_y);

  if (ady <= adx) {
    int p = 2 * ady - adx;
    for (int i = 0; i < adx; i++) {
      x += sx;
      if (p < 0) {
        p += 2 * ady;
      } else {
        y += sy;
        p += 2 * (ady - adx);
      }
      points.push_back({x, y});
    }
  } else {
    int p = 2 * adx - ady;
    for (int i = 0; i < ady; i++) {
      y += sy;
      if (p < 0) {
        p += 2 * adx;
      } else {
        x += sx;
        p += 2 * (adx - ady);
      }
      points.push_back({x, y});
    }
  }
  return points;
}

std::vector<std::pair<int, int>> MidPointCircle(int xc, int yc, int r) {
  std::vector<std::pair<int, int>> points;
  int x = 0, y = r;
  int p = 1 - r;
  points.push_back({xc, yc + r});
  points.push_back({xc, yc - r});
  points.push_back({xc + r, yc});
  points.push_back({xc - r, yc});
  while (x < y) {
    x += 1;
    if (p < 0) {
      p += 2 * x + 1;
    } else {
      y += -1;
      p += 2 * x + 1 - 2 * y;
    }

    points.push_back({xc + x, yc + y});
    points.push_back({xc + y, yc + x});
    points.push_back({xc - x, yc + y});
    points.push_back({xc - y, yc + x});
    points.push_back({xc - x, yc - y});
    points.push_back({xc - y, yc - x});
    points.push_back({xc + x, yc - y});
    points.push_back({xc + y, yc - x});
  }
  return points;
}

std::vector<std::pair<int, int>> Histrogram(std::vector<int> &data) {
  int fx = 10, fy = 10;
  int y = *std::max_element(data.begin(), data.end());
  int x = data.size();

  const int initx = 100;
  const int inity = 100;

  std::vector<std::pair<int, int>> axes;
  std::vector<std::pair<int, int>> axes1 =
      BressenhamLineDrawing(initx, inity, initx, y + inity * 2);
  std::vector<std::pair<int, int>> axes2 =
      BressenhamLineDrawing(initx, inity, x * initx + initx * 2, inity);
  axes.insert(axes.end(), axes1.begin(), axes1.end());
  axes.insert(axes.end(), axes2.begin(), axes2.end());

  for (int i = 0; i < x; i++) {

    int barLeft = initx + i * initx;
    int barTop = data[i] + inity;
    int barRight = initx + (i + 1) * initx;

    std::vector<std::pair<int, int>> l1 =
        BressenhamLineDrawing(barLeft, inity, barLeft, barTop);

    std::vector<std::pair<int, int>> l2 =
        BressenhamLineDrawing(barLeft, barTop, barRight, barTop);

    std::vector<std::pair<int, int>> l3;
    for (auto &m : l1) {
      l3.push_back({m.first + initx, m.second});
    }
    axes.insert(axes.end(), l1.begin(), l1.end());
    axes.insert(axes.end(), l2.begin(), l2.end());
    axes.insert(axes.end(), l3.begin(), l3.end());
  }

  return axes;
}

std::vector<std::pair<int, int>> DrawPieChart(std::vector<int> data) {

  int total = std::accumulate(data.begin(), data.end(), 0);
  std::vector<float> degrees;
  std::vector<std::pair<int, int>> plottingPoints;

  for (auto &m : data)
    degrees.push_back((static_cast<float>(m) / total) * 360.0f);

  auto append = [&](std::vector<std::pair<int, int>> pts) {
    plottingPoints.insert(plottingPoints.end(), pts.begin(), pts.end());
  };

  int r = 100;
  int xc = 400;
  int yc = 400;

  append(MidPointCircle(xc, yc, r));
  append(BressenhamLineDrawing(xc, yc, xc + r, yc));

  float cumAngle = 0.0f;
  for (int i = 0; i < data.size(); i++) {
    cumAngle += degrees[i];
    float rad = cumAngle * M_PI / 180.0f;

    int xNew = xc + static_cast<int>(r * cos(rad));
    int yNew = yc + static_cast<int>(r * sin(rad));
    append(BressenhamLineDrawing(xc, yc, xNew, yNew));
  }
  return plottingPoints;
}

void drawShapes(GLFWwindow *window, unsigned int shaderProgram) {
  // std::vector<std::pair<int, int>> linePoints = DDA(100, 100, 500, 500);

  // std::vector<std::pair<int, int>> linePoints =
  //     BressenhamLineDrawing(100, 100, 500, 500);

  // std::vector<std::pair<int, int>> linePoints = MidPointCircle(400, 400,
  // 100);

  std::vector<int> data = {10, 20, 25, 15, 10};
  // auto linePoints = Histrogram(data);

  auto linePoints = DrawPieChart(data);

  unsigned int VAO, VBO;
  createMesh(linePoints, VAO, VBO);
  glPointSize(3.0f);
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, linePoints.size());
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}
