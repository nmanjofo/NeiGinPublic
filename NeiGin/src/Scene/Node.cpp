#include "Node.h"

using namespace Nei;

Node::Node() {
  type = staticType();
}
Node::~Node() {}

const char* Node::getTypeName() const {
  switch(type) {
    case NodeType::Special:
      return "Special";
    case NodeType::Node:
      return "Node";
    case NodeType::SimpleModel:
      return "SimpleModel";
    case NodeType::Model:
      return "Model";
    case NodeType::ModelInstance:
      return "ModelInstance";
    case NodeType::LodSwitch:
      return "LodSwitch";
    case NodeType::Camera:
      return "Camera";
    case NodeType::Light:
      return "Light";
    case NodeType::Mesh:
      return "Mesh";
    default: ;
      return "Unknown";
  }
}

void Node::setLocalMatrix(mat4 const& mat) {
  transformType = TransformType::Local;
  transformDirty = true;
  localMatrix = mat;
}

void Node::updateAABB() {
  aabb = AABB();
  for(auto& c : children) {
    aabb.extend(c->getAABB());
  }
}

void Node::add(Ptr<Node> const& node) {
  node->removeParent();
  children.push_back(node);
  node->parent = this;
}

void Node::remove(Ptr<Node> const& node) {
  auto it = std::find(children.begin(), children.end(), node);
  if(it != children.end()) {
    children.erase(it);
    node->parent = nullptr;
  }
}

void Node::setParent(Ptr<Node> const& node) {
  node->add(this);
}

void Node::removeParent() {
  if(parent)
    parent->remove(this);
}

void Node::updateTransform() {
  transformUpdated = false;

  switch(transformType) {
    case TransformType::None:
      if(parent)
        worldMatrix = parent->getWorldMatrix();
      else
        worldMatrix = mat4(1);

      break;
    case TransformType::Local:
      if(parent && parent->wasDirty() || isDirty()) {
        //nei_log("transform update");
        worldMatrix = parent->getWorldMatrix() * localMatrix;
        transformUpdated = true;
      } else {
        // nei_log("transform skipped");
      }
      break;
    case TransformType::World:
      // worldMatrix remains
      transformUpdated = transformDirty;
      break;
    default: ;
  }

  transformDirty = false;
}
