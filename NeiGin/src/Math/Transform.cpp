
#include "Transform.h"

#include "glm/gtx/quaternion.hpp"

using namespace Nei;

const Nei::Transform Nei::Transform::identity;

mat4 Transform::getMatrix() const {
  mat4 ret(1);
  ret = translate(ret, position);
  ret = ret * toMat4(orientation);
  ret = glm::scale(ret, scale);
  return ret;
}
