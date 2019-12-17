#pragma once

#include "NeiGinBase.h"
#include "Node.h"

namespace Nei{
  enum class CameraProjection {
    Perspective,
    Parallel
  };

  class NEIGIN_EXPORT Camera : public Node {
  public:
    Camera(CameraProjection projectionType = CameraProjection::Perspective);
    virtual ~Camera();
    static NodeType staticType(){return NodeType::Camera;}
    
    void setFov(float fovy, float aspect);
    void setFov(float fovy, ivec2 size);
    void setFov(vec2 fov);

    float getAspect() const {return aspect;}
    vec2 getFov() const {return fov;}
    
    mat4 const& getProjection();
    mat4 const& getView(){return view;}
    void setView(mat4 const&view);

    vec3 eye; // temp

  protected:
    void computeProjection();

    mat4 view;

    // projection
    CameraProjection projectionType;
    mat4 projection;
    bool projectionDirty = true;

    vec2 fov;
    float aspect;
    float nearPlane = 0.1f;
    float farPlane = 10000;
  };
};


