#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

#include "Vertex.h"
#include "Mesh.h"

class Human {
  public:
    Human();
    ~Human();

    vector<Vertex> vertices;
    vector<Vertex> normals;
    vector<Mesh> meshes;

    vector<Vertex> joints;

    void exportToOBJ();
};
