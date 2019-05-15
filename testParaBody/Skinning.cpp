#include "Skinning.h"

Skinning::Skinning() {

}

Skinning::Skinning(Human& human) {
  vertices = &human.vertices;
  meshes = &human.meshes;

  joints = &human.joints;
}

Skinning::~Skinning() {

}

void Skinning::segment() {
  vector<Vertex> tmpJointGroup;
  for (int i = 0; i < joints->size(); i++) {
    Vertex v = (*joints)[i];
    v.idx = i;
    tmpJointGroup.push_back(v);
  }

  /*** Finding closest joint does NOT produce perfect segmentation ***/
  for (int i = 0; i < vertices->size(); i++) {
    Vertex closestJoint = (*vertices)[i].closest(tmpJointGroup);
    weightSegment[closestJoint.idx].push_back(i); // Push real vertex index for the benefit of access
  }
  /****** Only assign group only if two of my neighbors are in the same group ******/
  /********************************/

  for (int i = 0; i < weightSegment[Joint_shoulderMid].size(); i++ ) {
    Vertex v = (*vertices)[weightSegment[Joint_shoulderMid][i]];
    weightSegment[Joint_shoulderR].push_back(weightSegment[Joint_shoulderMid][i]);
  }

  /*** Filter Joint_shoulderR weight segment***/
  vector<int> tmpW;
  for (int i = 0; i < weightSegment[Joint_shoulderR].size(); i++) {
    Vertex v = (*vertices)[weightSegment[Joint_shoulderR][i]];
    
    if (v.x >= (*joints)[Joint_shoulderR].x - 0.3) {
      if (v.x < (*joints)[Joint_shoulderMid].x) {
          tmpW.push_back(weightSegment[Joint_shoulderR][i]);
      }
    }
    else {
      tmpW.push_back(weightSegment[Joint_shoulderR][i]);
    }
  }

  weightSegment[Joint_shoulderR].clear();
  weightSegment[Joint_shoulderR].insert(weightSegment[Joint_shoulderR].end(), tmpW.begin(), tmpW.end());

  armRSegment.insert(armRSegment.end(), weightSegment[Joint_shoulderR].begin(), weightSegment[Joint_shoulderR].end());
  armRSegment.insert(armRSegment.end(), weightSegment[Joint_elbowR].begin(), weightSegment[Joint_elbowR].end());
  armRSegment.insert(armRSegment.end(), weightSegment[Joint_wristR].begin(), weightSegment[Joint_wristR].end());
}

void Skinning::paintWeight() {
  float weightRange = 0.3;

  /*** Upper Arm R ***/
  for (int i = 0; i < armRSegment.size(); i++) {
    Vertex* v = &(*vertices)[armRSegment[i]];

    if (v->x > (*joints)[Joint_shoulderR].x - weightRange) {
         if (v->y > (*joints)[Joint_shoulderR].y && v->distance((*joints)[Joint_shoulderR]) < 0.5 ) {
           v->jointsRelated.push_back(Joint_shoulderR);
           v->jointWeights.push_back(1);
         }
         else {
           float dist = v->distance((*joints)[Joint_shoulderMid]);
           v->jointsRelated.push_back(Joint_shoulderMid);
           v->jointWeights.push_back(pow(1/dist, 4));

           dist = v->distance((*joints)[Joint_shoulderR]);
           v->jointsRelated.push_back(Joint_shoulderR);
           v->jointWeights.push_back(pow(1/dist, 4));

           dist = v->distanceToLine((*joints)[Joint_waist], (*joints)[Joint_shoulderMid]);
           v->jointsRelated.push_back(Joint_waist);
           v->jointWeights.push_back(pow(1/dist, 4));
         }
    }
    else {
      v->jointsRelated.push_back(Joint_shoulderR);
      v->jointWeights.push_back(1);
    }
  }
  /*******************/

  for (int i = 0; i < armRSegment.size(); i++) {
    Vertex* v = &(*vertices)[armRSegment[i]];

    float q = 0;
    for (int j = 0; j < v->jointWeights.size(); j++) {
      q += v->jointWeights[j];
    }
    for (int j = 0; j < v->jointWeights.size(); j++) {
      v->jointWeights[j] /= q;
    }
  }
}

void Skinning::rotate(int part, float degree) {
  float thisRad = (degree/100) * M_PI / 180;
  float radian = degree * M_PI/180;

  Vertex pivotJoint = (*joints)[Joint_shoulderMid];

  for (int i = 0; i < armRSegment.size(); i++) {
    Vertex* v = &(*vertices)[armRSegment[i]];

    float tmp_x = 0, tmp_y = 0;
    float x = v->x - pivotJoint.x;;
    float y = v->y - pivotJoint.y;

    for (int j = 0; j < v->jointsRelated.size(); j++) {
      Vertex jt = (*joints)[v->jointsRelated[j]];

      if (v->jointsRelated[j] == part) {
        tmp_x += v->jointWeights[j] * (cos(radian) * (x) - sin(radian) * (y));
        tmp_y += v->jointWeights[j] * (sin(radian) * (x) + cos(radian) * (y));
      }
      else {
        tmp_x += v->jointWeights[j] * (cos(thisRad) * (x) - sin(thisRad) * (y));
        tmp_y += v->jointWeights[j] * (sin(thisRad) * (x) + cos(thisRad)  * (y ));
      }
    }

    v->x = tmp_x + pivotJoint.x;
    v->y = tmp_y + pivotJoint.y;
  }
}
