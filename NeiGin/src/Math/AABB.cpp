#include "AABB.h"

using namespace Nei;

AABB::AABB() {
}

AABB::AABB(vec3 const& min, vec3 const& max): min(min), max(max) {
}
