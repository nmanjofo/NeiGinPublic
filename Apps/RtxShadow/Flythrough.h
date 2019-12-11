#pragma once

#include "NeiGinBase.h"

struct FlyPoint
{
  float time;
  vec3 position;
  vec3 fwd;
  vec3 up;

  mat4 getMat()
  {
    return glm::lookAt(position, position + fwd, up);
  }
};

class Flythrough {
public:

  void setSpeed(float speed);
  mat4 sample(float time);
  mat4 sample(int frame, int totalFrames);

  std::vector<FlyPoint> points;
};