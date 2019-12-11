#pragma once

#include "NeiGinBase.h"
#include "Math/Transform.h"

namespace Nei{
  class NEIGIN_EXPORT OrbitManipulator : public Object {
  public:
    OrbitManipulator(Camera* camera = nullptr);
    virtual ~OrbitManipulator();

    void update();

    auto const& getEye() const {return eye;}
  protected:
    void initCallbacks();
    
    bool enabled=true;    
    Ptr<Camera> camera;
    Ptr<Node> follow;
    Transform followOffset;

    int button=1;
    float zoom = 5;
    vec2 rotation;
    vec2 speed = vec2(0.2,0.2);
    bool pressed=false;

    vec3 center;
    vec3 eye;
    vec3 up = vec3(0,1,0);
  };
};


