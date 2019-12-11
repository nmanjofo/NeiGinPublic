#pragma once

#include "Pipeline.h"

namespace Nei::Vu {
  class NEIVU_EXPORT RaytracingPipeline : public Pipeline {
  public:
    RaytracingPipeline(DeviceContext* deviceContext);
    virtual ~RaytracingPipeline();

    void bind(CommandBuffer* cmd) override;
    void create();

    void addRayGenShader(Shader* shader);
    void addMissShader(Shader* shader);
    void addHitShader(Shader* closest, Shader* any, Shader* intersection);
    void addCallableShader(Shader* shader){};

    auto &getHitGroups() const { return hitGroups; }

    Ptr<ShaderBindingTable> createShaderBindingTable();

    void setRecursionDepth(int d){recursionDepth=d;}
    
    vk::PipelineBindPoint getBindPoint() const override { return vk::PipelineBindPoint::eRayTracingNV; }

    operator vk::Pipeline() const { return pipeline; }
    vk::Pipeline operator*() const { return pipeline; }
  protected:
    vk::Pipeline pipeline;
    std::vector<vk::RayTracingShaderGroupCreateInfoNV> hitGroups;

    int recursionDepth = 1;
  };

};
