#include "Camera.h"

using namespace Nei;

Camera::Camera(CameraProjection projectionType): projectionType(projectionType) {
  type = staticType();
  setFov(90, { 1024, 768 });
}

Camera::~Camera() { }

void Camera::setFov(float fovy, float aspect) {
  this->aspect = aspect;
  float fovx = degrees(2 * atan(tan(radians(fovy) * 0.5f) * aspect));
  fov = vec2(fovx,fovy);
  projectionDirty = true;
}

void Camera::setFov(float fovy, ivec2 size) {
  setFov(fovy,float(size.x)/float(size.y));
}

void Camera::setFov(vec2 fov) {
  this->fov = fov;
  //aspect = glm::tan( glm::radians( fov.x * 0.5f ) ) / glm::tan( glm::radians( fov.y * 0.5f ) );
  projectionDirty = true;
}

mat4 const& Camera::getProjection() {
  if (projectionDirty) computeProjection();
  return projection;
}

void Camera::setView(mat4 const& view) {
  this->view = view;
}

void Camera::computeProjection() {
  if (projectionType == CameraProjection::Perspective) {
    if(isnan(aspect)||isnan(fov.y)||isnan(fov.x)) return;
    projection = perspective(radians(fov.y), aspect, nearPlane, farPlane);
    projectionDirty = false;
  }
}
