#include "lab4.h"
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

int computeCode(double x, double y, double xmin, double ymin, double xmax,
                double ymax) {
  int code = INSIDE;

  if (x < xmin)
    code |= LEFT;
  else if (x > xmax)
    code |= RIGHT;

  if (y < ymin)
    code |= BOTTOM;
  else if (y > ymax)
    code |= TOP;

  return code;
}

void ClipLines(GLFWwindow *window, unsigned int shaderProgram) {
  int x1 = 10;
  int y1 = 10;
  int x2 = 10;
  int y2 = 10;

  int p1 = computeCode(x1, y1, 20, 20, 40, 40);
  int p2 = computeCode(x2, y2, 20, 20, 40, 40);

  if (p1 == p2 == 0) {
    // accpet it whole
  }

  if (p1 & p2 != 0) {
    // reject line
  } else {
  }
}
