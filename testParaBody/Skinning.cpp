#include "Skinning.h"

Skinning::Skinning() {

}

Skinning::Skinning(Human& human) {
  vertices = &human.vertices;
  meshes = &human.meshes;
  normals = &human.normals;
  joints = &human.joints;
  bones = &human.bones;
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
    Vertex closest = (*vertices)[i].closest(tmpJointGroup);
    weightSegment[closest.idx].push_back(i); // Push real vertex index for the benefit of access
  }
  /********************************/

  /*** ElbowR ***/
  vector<int> tmpElbowR;
  for (int i = 0; i < weightSegment[Joint_elbowR].size(); i++) {
    Vertex v = (*vertices)[weightSegment[Joint_elbowR][i]];
    Vertex shoulderR = (*joints)[Joint_shoulderR];
    if ( v.x < shoulderR.x ) {
      tmpElbowR.push_back(weightSegment[Joint_elbowR][i]);
    }
  }
  weightSegment[Joint_elbowR].clear();
  weightSegment[Joint_elbowR].insert(weightSegment[Joint_elbowR].end(), tmpElbowR.begin(), tmpElbowR.end());
  /**************/

  /*** ShoulderR ***/
  vector<int> tmpShoulderR;
  for (int i = 0; i < weightSegment[Joint_shoulderR].size(); i++) {
    Vertex v = (*vertices)[weightSegment[Joint_shoulderR][i]];
    Vertex shoulderMid = (*joints)[Joint_shoulderMid];
    Vertex shoulderR = (*joints)[Joint_shoulderR];

    if ( v.x < shoulderR.x - 0.3) {
      tmpShoulderR.push_back(weightSegment[Joint_shoulderR][i]);
    }
    else {
      if (v.y > shoulderR.y - 0.7 && v.x < shoulderMid.x) {
        tmpShoulderR.push_back(weightSegment[Joint_shoulderR][i]);
      }
    }
  }

  weightSegment[Joint_shoulderR].clear();
  weightSegment[Joint_shoulderR].insert(weightSegment[Joint_shoulderR].end(), tmpShoulderR.begin(), tmpShoulderR.end());
  /*****************/
  

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
      //if (v->x < (*joints)[Joint_shoulderMid].x - 1.3) {
        //float dist = v->distance((*joints)[Joint_shoulderMid]);
        Vertex shoulderMid = (*joints)[Joint_shoulderMid];
        Vertex shoulderR = (*joints)[Joint_shoulderR];
        float dist = v->distance((shoulderMid + shoulderR*2)/3);
        v->jointsRelated.push_back(Joint_shoulderMid);
        v->jointWeights.push_back(pow(1/dist, 4));

        dist = v->distance((*joints)[Joint_shoulderR]);
        v->jointsRelated.push_back(Joint_shoulderR);
        v->jointWeights.push_back(pow(1/dist, 4));

        dist = v->distance(Vertex(shoulderR.x, shoulderR.y - 0.1, shoulderR.z));
        v->jointsRelated.push_back(Joint_waist);
        v->jointWeights.push_back(pow(1/dist, 4));
        //dist = v->distanceToLine((*joints)[Joint_shoulderMid], (*joints)[Joint_waist]);
        //v->jointsRelated.push_back(Joint_waist);
        //v->jointWeights.push_back(pow(1/dist, 4));
      //}
      //else {
        //float dist = v->distanceToLine((*joints)[Joint_shoulderMid], (*joints)[Joint_shoulderR]);
        //v->jointsRelated.push_back(Joint_shoulderMid);
        //v->jointWeights.push_back(pow(1/dist, 4));

        //dist = v->distance((*joints)[Joint_shoulderR]);
        //v->jointsRelated.push_back(Joint_shoulderR);
        //v->jointWeights.push_back(pow(1/dist, 4));
      //}
    }
    else {
      v->jointsRelated.push_back(Joint_shoulderR);
      v->jointWeights.push_back(1);
    }
  }

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

  /*******************/
}

void Skinning::rotateRA(int part, float degree) {
  float thisRad = (0/10) * M_PI / 180;
  float radian = degree * M_PI/180;

  Vertex pivotJoint;

  int jrs[3] = {Joint_shoulderR, Joint_elbowR, Joint_wristR};

  for (int i = 0; i < 3; i++ ) {
    float jx = (*joints)[jrs[i]].x;
    float jy = (*joints)[jrs[i]].y;

    if ( jrs[i] == Joint_shoulderR ) {
      //pivotJoint = (*joints)[Joint_shoulderMid];

      //jx -= pivotJoint.x;
      //jy -= pivotJoint.y;

      //(*joints)[jrs[i]].x = cos(thisRad) * jx - sin(thisRad) * jy;
      //(*joints)[jrs[i]].y = cos(thisRad) * jx + sin(thisRad) * jy;

      //(*joints)[jrs[i]].x += pivotJoint.x;
      //(*joints)[jrs[i]].y += pivotJoint.y;
    }
    else {
      pivotJoint = (*joints)[Joint_shoulderR];

      jx -= pivotJoint.x;
      jy -= pivotJoint.y;

      (*joints)[jrs[i]].x = cos(radian) * jx - sin(radian) * jy;
      (*joints)[jrs[i]].y = cos(radian) * jx + sin(radian) * jy;

      (*joints)[jrs[i]].x += pivotJoint.x;
      (*joints)[jrs[i]].y += pivotJoint.y;
    }
  }

  for (int i = 0; i < armRSegment.size(); i++) {
    Vertex* v = &(*vertices)[armRSegment[i]];

    float x = 0, y = 0;
    float tmp_x = 0, tmp_y = 0;


    for (int j = 0; j < v->jointsRelated.size(); j++) {
      if (v->jointsRelated[j] == Joint_shoulderMid) {
        pivotJoint = (*joints)[Joint_shoulderMid];
        x = v->x - pivotJoint.x;
        y = v->y - pivotJoint.y;
        tmp_x += v->jointWeights[j] * (cos(thisRad) * (x) - sin(thisRad) * (y) + pivotJoint.x);
        tmp_y += v->jointWeights[j] * (sin(thisRad) * (x) + cos(thisRad)  * (y) + pivotJoint.y);
      }
      else  {
        pivotJoint = (*joints)[Joint_shoulderR];
        x = v->x - pivotJoint.x;
        y = v->y - pivotJoint.y;
        tmp_x += v->jointWeights[j] * (cos(radian) * (x) - sin(radian) * (y) + pivotJoint.x);
        tmp_y += v->jointWeights[j] * (sin(radian) * (x) + cos(radian) * (y) + pivotJoint.y);
      }
    }
    v->x = tmp_x;
    v->y = tmp_y;
  }
}
