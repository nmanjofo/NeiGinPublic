#pragma once

#include "NeiGinBase.h"

namespace Nei{
  class NEIGIN_EXPORT Transform{
  public:
    /*Transform(){}
    virtual ~Transform(){}
    Transform(Transform const& t);
    Transform(Transform const&& t) noexcept;*/

    mat4 getMatrix() const;
    
    vec3 position = vec3(0,0,0);
    vec3 scale = vec3(1, 1, 1);
    quat orientation = quat(1,0,0,0);

    static const Transform identity;
  };
};


