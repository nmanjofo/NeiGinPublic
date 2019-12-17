#include "NeiGin.h"
#include "Args.h"
#include "Loader.h"
#include "CameraPath.h"
#include "Loader.h"
#include "Profiler.h"
#include "Scene/RaytracingBVH.h"

using namespace Nei;
using namespace Vu;

class MainApp: public Nei::SimpleApplication {
public:
  MainApp(int argc, char** argv);

  void update(Nei::AppFrame const& frame) override;
  void draw() override;

protected:
  const int skipFrames = 60;

  Args args;
  ModelData model;
  CameraPath cameraPath;
  vec3 lightPosition = { 0,10,0 };

  uvec2 resolution;

  Ptr<RaytracingBVH> bvh;
  Ptr<ShaderBindingTable> sbt;
  Ptr<GraphicsPipeline> gbufferPipeline;
  Ptr<RaytracingPipeline> shadowMaskPipeline;
  Ptr<ComputePipeline> lightingPipeline;

  Ptr<DescriptorSet> gbufferDescriptor;
  Ptr<DescriptorSet> shadowMaskDescriptor;
  Ptr<DescriptorSet> lightingDescriptor;

  Ptr<GBuffer> gbuffer;
  Ptr<Texture2D> shadowMask;
  Ptr<Texture2D> accBuffer;
  Ptr<Profiler> profiler;

  Ptr<CommandBuffer> commandBuffers[4];
  int currentFrame = 0;
};



