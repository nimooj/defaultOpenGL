#pragma once

#include <vector>

#include "JointNames.h"
#include "Human.h"

#include "Vertex.h"
#include "Mesh.h"
#include "Bone.h"

#include <cmath>
#define _USE_MATH_DEFINES

class Skinning {
  public :
    Skinning();
    Skinning(Human&);
    ~Skinning();

    vector<Vertex>* vertices;
    vector<Vertex>* normals;
    vector<Mesh>* meshes;

    vector<Vertex>* joints;
    vector<Bone>* bones;

    vector<int> jointGroup[JointNum];
    vector<int> weightSegment[JointNum];
    vector<int> bodySegment[BodyNum];

    void segment();
    void paintWeight();
    void rotateRA(int, float);

    vector<int> armRSegment;
    vector<int> elbowRSegment;
    vector<int> handRSegment;
};
