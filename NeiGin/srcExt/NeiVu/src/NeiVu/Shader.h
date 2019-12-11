#pragma once

#include "NeiVuBase.h"

namespace Nei::Vu {
  class NEIVU_EXPORT Shader : public Object {
  public:
    Shader(DeviceContext* dc);
    Shader(DeviceContext* dc, std::string const& src, vk::ShaderStageFlagBits stage, std::string const&name="");
    virtual ~Shader();

    bool fromString(std::string const& src, vk::ShaderStageFlagBits stage, std::string const& name="");
    bool fromFile(std::string const& fileName);

    vk::ShaderModule operator*() const {return module;}
    operator vk::ShaderModule() const {return module;}

    auto & getLayouts() const { return layouts; }
    auto & getPushConstants() const { return pushConstants; }

    auto getStage() const{return stage;}

    void addResourcesToPipeline(Pipeline* pipeline);
  protected:
    Ptr<DeviceContext> deviceContext;
    std::vector<uint32> spirv;
    vk::ShaderModule module;
    vk::ShaderStageFlagBits stage;

    std::vector<Ptr<DescriptorSetLayout>> layouts;
    std::vector<vk::PushConstantRange> pushConstants;
  };
};
