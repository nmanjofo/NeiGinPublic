#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT Pipeline : public DeviceObject {
  public:
    Pipeline(DeviceContext* deviceContext);
    virtual ~Pipeline();

    void addShader(Shader* shader, const char* main = "main");
    void addStage(vk::ShaderStageFlagBits stage, vk::ShaderModule shader, const char* main = "main");
    void addLayout(DescriptorSetLayout* layout);
    void addPushConstantRange(uint32_t size, uint32_t offset = 0,
                              vk::ShaderStageFlags stage = vk::ShaderStageFlagBits::eAll);

    auto & getShaders() const{ return shaders; }

    virtual void bind(CommandBuffer* cmd) = 0;

    Ptr<DescriptorSet> allocateDescriptorSet(int set = 0);

    vk::PipelineLayout getLayout() const { return pipelineLayout; }

    Ptr<DescriptorSetLayout> getOrCreateDescriptorSetLayout(int i);

    virtual vk::PipelineBindPoint getBindPoint() const =0;

    void createLayout();

    template<typename T>
    void setConstants(CommandBuffer* cmd, T const& value, uint offset = 0, vk::ShaderStageFlags stage = vk::ShaderStageFlagBits::eVertex);

  protected:
    std::vector<Ptr<Shader>> shaders;

    std::vector<Ptr<DescriptorSetLayout>> layouts;
    std::vector<Ptr<DescriptorPool>> descriptorPools;

    std::vector<vk::PipelineShaderStageCreateInfo> stages;
    std::vector<vk::PushConstantRange> pushConstants;
    vk::PipelineLayout pipelineLayout;
  };

  template <typename T>
  void Pipeline::setConstants(CommandBuffer* cmd, T const& value, uint offset, vk::ShaderStageFlags stage) {
    (**cmd).pushConstants(pipelineLayout, stage, offset, sizeof(T), &value);
  }
};
