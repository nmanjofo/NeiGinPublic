#include "OrbitManipulator.h"
#include "Application/Application.h"
#include "Application/Window.h"

#include "Camera.h"

using namespace Nei;

OrbitManipulator::OrbitManipulator(Camera* camera):camera(camera) {
  initCallbacks();
}

OrbitManipulator::~OrbitManipulator() { }

void OrbitManipulator::update() {
  if(camera) {
    eye = vec3(-zoom, 0, 0);
    eye = rotate(eye, radians(rotation.y), vec3(0, 0, 1));
    eye = rotate(eye, radians(rotation.x), vec3(0, 1, 0));
    camera->setView(lookAt(eye,center,up));
    camera->eye = eye;
  }
}

void OrbitManipulator::initCallbacks() {
      LISTEN2(WindowMessage,{
    switch(msg.type) {
    case WindowMessage::MessageType::MouseMove:
      if(pressed) {
        rotation+=vec2(msg.delta)*speed;
        rotation.y=clamp(rotation.y,-89.f,89.f);
      }
      break;
    case WindowMessage::MessageType::MouseButton:
      if(msg.button==button) {
        pressed=msg.press;
      }
      break;
    case WindowMessage::MessageType::MouseWheel:
      zoom*= pow(1.1f,-msg.delta.y);
      break;
    case WindowMessage::MessageType::Key:
      break;
    default: ;
    }

  });
      }
