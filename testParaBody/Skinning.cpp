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

  for (int i = 0; i < vertices->size(); i++) {
    Vertex closestJoint = (*vertices)[i].closest(tmpJointGroup);
    weightSegment[closestJoint.idx].push_back(i); // Push real vertex index for the benefit of access
  }

  vector<int> tmpW;
  for (int i = 0; i < weightSegment[Joint_shoulderR].size(); i++ ) {
    Vertex v = (*vertices)[weightSegment[Joint_shoulderR][i]];
    if (v.x >= (*joints)[Joint_shoulderR].x ) {
      if ( v.distance((*joints)[Joint_shoulderR]) <= 0 )
        tmpW.push_back(weightSegment[Joint_shoulderR][i]);
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

  elbowRSegment.insert(elbowRSegment.end(), weightSegment[Joint_elbowR].begin(), weightSegment[Joint_elbowR].end());
  elbowRSegment.insert(elbowRSegment.end(), weightSegment[Joint_wristR].begin(), weightSegment[Joint_wristR].end());

  handRSegment.insert(handRSegment.end(), weightSegment[Joint_wristR].begin(), weightSegment[Joint_wristR].end());
}

void Skinning::paintWeight() {
  float weightRange = 0.2;

  /*** Upper Arm R ***/
  for (int i = 0; i < armRSegment.size(); i++) {
    Vertex* v = &(*vertices)[armRSegment[i]];

    if (v->x > (*joints)[Joint_shoulderR].x - weightRange) {
      /*
         if (v->y > (*joints)[Joint_shoulderR].y && v->distance((*joints)[Joint_shoulderR]) < 0.5 ) {
         v->jointsRelated.push_back(Joint_shoulderR);
         v->jointWeights.push_back(1);
         }
         else {
         float dist = v->distanceToLine((*joints)[Joint_shoulderMid], (*joints)[Joint_shoulderR]);
         v->jointsRelated.push_back(Joint_shoulderMid);
         v->jointWeights.push_back(pow(1/dist, 4));

         dist = v->distanceToLine((*joints)[Joint_shoulderR], (*joints)[Joint_elbowR]);
         v->jointsRelated.push_back(Joint_shoulderR);
         v->jointWeights.push_back(pow(1/dist, 4));
         }
         */
      //float dist = v->distanceToLine((*joints)[Joint_shoulderMid], (*joints)[Joint_shoulderR]);
      float dist = v->distance((*joints)[Joint_shoulderMid]);
      v->jointsRelated.push_back(Joint_shoulderMid);
      v->jointWeights.push_back(pow(1/dist, 4));

      //dist = v->distanceToLine((*joints)[Joint_shoulderR], (*joints)[Joint_elbowR]);
      dist = v->distance((*joints)[Joint_shoulderR]);
      v->jointsRelated.push_back(Joint_shoulderR);
      v->jointWeights.push_back(pow(1/dist, 4));
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
  float thisRad = -2 * M_PI / 180;

  float radian = degree * M_PI/180;

  for (int i = 0; i < armRSegment.size(); i++) {
    Vertex* v = &(*vertices)[armRSegment[i]];

    float x = v->x;
    float y = v->y;

    float tmp_x = 0, tmp_y = 0;
    for (int j = 0; j < v->jointsRelated.size(); j++) {
      Vertex jt = (*joints)[v->jointsRelated[j]];

      if (v->jointsRelated[j] == 1) { // Shoulder Mid -> no rotation
        tmp_x += v->jointWeights[j] * (cos(thisRad) * (x - jt.x) - sin(thisRad) * (y - jt.y));
        tmp_y += v->jointWeights[j] * (sin(thisRad) * (x - jt.x) + cos(thisRad)  * (y - jt.y));
      }
      else if (v->jointsRelated[j] == 2) { // Shoulder R -> use rotation Matrix
        tmp_x += v->jointWeights[j] * (cos(thisRad + radian) * (x - jt.x) - sin(thisRad + radian) * (y - jt.y));
        tmp_y += v->jointWeights[j] * (sin(thisRad + radian) * (x - jt.x) + cos(thisRad + radian) * (y - jt.y));
      }

    }

    v->x = tmp_x + (*joints)[Joint_shoulderR].x;
    v->y = tmp_y + (*joints)[Joint_shoulderR].y;
  }
}
