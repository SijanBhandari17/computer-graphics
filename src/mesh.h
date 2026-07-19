#ifndef MESH_H
#define MESH_H
#include <vector>

void createMesh(const std::vector<std::pair<int, int>> &points,
                unsigned int &VAO, unsigned int &VBO);

#endif
