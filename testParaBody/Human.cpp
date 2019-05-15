#include "Human.h"

Human::Human() {
  int id = 1;
  int meshId = 1;
  string line;
  ifstream infile("Body.obj");

  while( getline(infile, line) ) {
    istringstream iss(line);
    string type;
    float x, y, z;

    iss >> type >> x >> y >> z;

    if (type == "v") {
      vertices.push_back(Vertex(id, x, y, z));
      id++;
    }
    else if (type == "f") {
      meshes.push_back(Mesh(meshId, vertices[x - 1], vertices[y - 1], vertices[z- 1]));
      meshId += 1;
    }
  }
  infile.close();

  id = 1;
  ifstream infile1("joints.txt");
  while( getline(infile1, line) ) {
    istringstream iss(line);
    float x, y, z;

    iss >> x >> y >> z;
    joints.push_back(Vertex(id, x, y, z));
    id++;
  }
  infile1.close();

  for (int i = 0; i < meshes.size(); i++) {
    Vertex* v1 = &vertices[meshes[i].index1 - 1];
    Vertex* v2 = &vertices[meshes[i].index2 - 1];
    Vertex* v3 = &vertices[meshes[i].index3 - 1];

    v1->inMeshes.push_back(meshes[i].id);
    v2->inMeshes.push_back(meshes[i].id);
    v3->inMeshes.push_back(meshes[i].id);

    v1->nx += meshes[i].normal.x;
    v1->ny += meshes[i].normal.y;
    v1->nz += meshes[i].normal.z;
    v2->nx += meshes[i].normal.x;
    v2->ny += meshes[i].normal.y;
    v2->nz += meshes[i].normal.z;
    v3->nx += meshes[i].normal.x;
    v3->ny += meshes[i].normal.y;
    v3->nz += meshes[i].normal.z;

    int exists1 = 0, exists2 = 0, exists3 = 0;
    for (int j = 0; j < v1->neighbors.size(); j++) {
      if (meshes[i].index2 == v1->neighbors[j])
        exists2 = 1;
      if (meshes[i].index3 == v1->neighbors[j])
        exists3 = 1;
    }
    if (exists2 == 0)
      v1->neighbors.push_back(meshes[i].index2);
    if (exists3 == 0)
      v1->neighbors.push_back(meshes[i].index3);

    exists1 = 0, exists2 = 0, exists3 = 0;
    for (int j = 0; j < v2->neighbors.size(); j++) {
      if (meshes[i].index1 == v2->neighbors[j])
        exists1 = 1;
      if (meshes[i].index3 == v2->neighbors[j])
        exists3 = 1;
    }
    if (exists1 == 0)
      v2->neighbors.push_back(meshes[i].index1);
    if (exists3 == 0)
      v2->neighbors.push_back(meshes[i].index3);

    exists1 = 0, exists2 = 0, exists3 = 0;
    for (int j = 0; j < v3->neighbors.size(); j++) {
      if (meshes[i].index1 == v3->neighbors[j])
        exists1 = 1;
      if (meshes[i].index2 == v3->neighbors[j])
        exists2 = 1;
    }
    if (exists1 == 0)
      v3->neighbors.push_back(meshes[i].index1);
    if (exists2 == 0)
      v3->neighbors.push_back(meshes[i].index2);
  }

  for (int i = 0; i < vertices.size(); i++) {
    vertices[i].nx /= vertices[i].inMeshes.size();
    vertices[i].ny /= vertices[i].inMeshes.size();
    vertices[i].nz /= vertices[i].inMeshes.size();

    normals.push_back(Vertex(vertices[i].nx, vertices[i].ny, vertices[i].nz));
  }
}

Human::~Human() {

}

void Human::exportToOBJ() {
  ofstream outfile("Result.obj");

  for (int i = 0; i < vertices.size(); i++) {
    outfile << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << endl;
  }
  for (int i = 0; i < meshes.size(); i++) {
    outfile << "f " << meshes[i].index1 << " " << meshes[i].index2 << " " << meshes[i].index3 << endl;
  }

  outfile.close();
  cout << "Obj exported." << endl;
}

