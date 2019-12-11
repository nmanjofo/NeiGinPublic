#pragma once

#include "NeiGinBase.h"

namespace Nei{
  class NEIGIN_EXPORT AABB{
  public:
    AABB();
    AABB(vec3 const& min, vec3 const& max);

    bool isValid() const { return min.x<=max.x && min.y<=max.y && min.z<=max.z;}

    void extend(vec3 const point) {
      min = glm::min(min,point);
      max = glm::max(max,point);
    }

    void extend(AABB const& aabb) {
      extend(aabb.min);
      extend(aabb.max);
    }

    vec3 min=vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max=vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
  };
};


