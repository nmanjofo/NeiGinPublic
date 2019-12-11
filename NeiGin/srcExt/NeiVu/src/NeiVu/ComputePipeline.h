#pragma once

#include "Pipeline.h"

namespace Nei::Vu {
  class NEIVU_EXPORT ComputePipeline : public Pipeline {
  public:
    ComputePipeline(DeviceContext* deviceContext);
    vk::PipelineBindPoint getBindPoint() const override { return vk::PipelineBindPoint::eCompute; }

    void create();

    vk::Pipeline operator*() const { return pipeline; }
    operator vk::Pipeline() const { return pipeline; }
    void bind(CommandBuffer* cmd) override;
  protected:
    vk::Pipeline pipeline;
  };
};
