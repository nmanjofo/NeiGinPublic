#pragma once

#include "NeiCore.h"
#include "NeiVu/NeiVuBase.h"

#include "Export.h"
#define SOL_ALL_SAFETIES_ON 1

namespace Nei {
  using namespace Vu;

  class Window;
  class ThreadWorker;
  class ThreadPool;

  // Math
  class Transform;
  class AABB;

  // Assets
  class Image;
  class ImageLoader;
  class AssetManager;
  class AssimpLoader;
  class ModelLoader;
  class MeshBuffer;

  // IO
  class VirtualFileSystem;

  // Scene
  class Node;
  class Camera;
  class Light;
  class TransformNode;
  class ModelInstance;
  class LodSwitch;
  class Scene;
  class OrbitManipulator;
  class SimpleModel;
  class ModelInstance;
  class Model;
  class Mesh;

  class Material;
  class PBRMaterial;

  class Visitor;
  class Renderer;
  class DeferredRenderer;
  class RendererCallback;

}

#define bit(x) (1<<x)

inline uint sizeAlign(uint v, uint a) {
  const uint na = a - 1;
  return (v + na) & ~na;
}
