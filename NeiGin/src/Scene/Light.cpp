#include "Light.h"

using namespace Nei;

Light::Light() {
  type = staticType();
}

Light::~Light() {

}

void Light::updateAABB() {
  Node::updateAABB();

  vec3 pos = getWorldMatrix()*vec4(0,0,0,1);
  aabb.extend(pos+vec3(radius));
  aabb.extend(pos-vec3(radius));
}
