#pragma once

#include <vector>

#include "JointNames.h"
#include "BodyGroups.h"
#include "Human.h"

#include "Vertex.h"
#include "Mesh.h"

class Skinning {
  public :
    Skinning();
    Skinning(Human&);
    ~Skinning();

    vector<Vertex>* vertices;
    vector<Mesh>* meshes;

    vector<Vertex>* joints;

    vector<int> jointGroup[18];

    vector<int> weightSegment[18];

    void segment();
    void paintWeight();
    void rotate(int);

  private :
    vector<int> armRSegment;
    vector<int> elbowRSegment;
    vector<int> handRSegment;
};
