#include "Shader.h"

#include "DeviceContext.h"
#include "DescriptorSetLayout.h"
#include "Pipeline.h"
#include "GraphicsPipeline.h"

#define NV_EXTENSIONS
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include <chrono>

using namespace Nei::Vu;

vk::Format attribFormat(spirv_cross::SPIRType::BaseType format, int vecSize) {
  switch(format) {
    case spirv_cross::SPIRType::Unknown:
      break;
    case spirv_cross::SPIRType::Void:
      break;
    case spirv_cross::SPIRType::Boolean:
      break;
    case spirv_cross::SPIRType::Char:
      break;
    case spirv_cross::SPIRType::SByte:
      break;
    case spirv_cross::SPIRType::UByte:
      break;
    case spirv_cross::SPIRType::Short:
      break;
    case spirv_cross::SPIRType::UShort:
      break;
    case spirv_cross::SPIRType::Int:
      break;
    case spirv_cross::SPIRType::UInt:
      break;
    case spirv_cross::SPIRType::Int64:
      break;
    case spirv_cross::SPIRType::UInt64:
      break;
    case spirv_cross::SPIRType::AtomicCounter:
      break;
    case spirv_cross::SPIRType::Half:
      break;
    case spirv_cross::SPIRType::Float:
      if(vecSize == 1) return vk::Format::eR32Sfloat;
      if(vecSize == 2) return vk::Format::eR32G32Sfloat;
      if(vecSize == 3) return vk::Format::eR32G32B32Sfloat;
      if(vecSize == 4) return vk::Format::eR32G32B32A32Sfloat;
      break;
    case spirv_cross::SPIRType::Double:
      break;
    case spirv_cross::SPIRType::Struct:
      break;
    case spirv_cross::SPIRType::Image:
      break;
    case spirv_cross::SPIRType::SampledImage:
      break;
    case spirv_cross::SPIRType::Sampler:
      break;
    case spirv_cross::SPIRType::ControlPointArray:
      break;
  }
  nei_error("attribFormat Unknown Format!");
  return vk::Format::eUndefined;
}

Shader::Shader(DeviceContext* dc): deviceContext(dc) { }

Shader::Shader(DeviceContext* dc, std::string const& src, vk::ShaderStageFlagBits stage,
               std::string const& name): Shader(dc) {
  fromString(src, stage, name.empty()?"unknown":name);
}

Shader::~Shader() {
  deviceContext->getVkDevice().destroyShaderModule(module);
  module = nullptr;
}

bool Shader::fromString(std::string const& src, vk::ShaderStageFlagBits stage, std::string const& name) {
  if(src.empty()) return false;
  this->stage = stage;

  auto start = std::chrono::high_resolution_clock::now();
  shaderc_shader_kind kind;
  switch(stage) {
    case vk::ShaderStageFlagBits::eVertex:
      kind = shaderc_vertex_shader;
      break;
    case vk::ShaderStageFlagBits::eTessellationControl:
      kind = shaderc_tess_control_shader;
      break;
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
      kind = shaderc_tess_evaluation_shader;
      break;
    case vk::ShaderStageFlagBits::eGeometry:
      kind = shaderc_geometry_shader;
      break;
    case vk::ShaderStageFlagBits::eFragment:
      kind = shaderc_fragment_shader;
      break;
    case vk::ShaderStageFlagBits::eCompute:
      kind = shaderc_compute_shader;
      break;
    case vk::ShaderStageFlagBits::eRaygenNV:
      kind = shaderc_raygen_shader;
      break;
    case vk::ShaderStageFlagBits::eMissNV:
      kind = shaderc_miss_shader;
      break;
    case vk::ShaderStageFlagBits::eAnyHitNV:
      kind = shaderc_anyhit_shader;
      break;
    case vk::ShaderStageFlagBits::eClosestHitNV:
      kind = shaderc_closesthit_shader;
      break;
    case vk::ShaderStageFlagBits::eCallableNV:
      kind = shaderc_callable_shader;
      break;
    case vk::ShaderStageFlagBits::eIntersectionNV:
      kind = shaderc_intersection_shader;
      break;
    default:
    nei_error("Invalid shader stage!");
      return false;;
  }

  shaderc::Compiler compiler;
  shaderc::CompileOptions opt;
  //opt.SetOptimizationLevel(shaderc_optimization_level_performance);

  auto res = compiler.CompileGlslToSpv(src, kind, name.c_str(), opt);

  auto const& er = res.GetErrorMessage();
  if(!er.empty()) {
    nei_error("Shader compile error:\n {}", er.c_str());
    return false;
  }

  spirv = std::vector<uint32>(res.begin(), res.end());

  vk::ShaderModuleCreateInfo smci;
  smci.codeSize = spirv.size() * sizeof(uint32);
  smci.pCode = spirv.data();
  module = deviceContext->getVkDevice().createShaderModule(smci);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> time = (end - start);
  nei_info("Compiling shader {} took {} ms", name, time.count());


  return !!module;
}

bool Shader::fromFile(std::string const& fileName) { return false; }

void Shader::addResourcesToPipeline(Pipeline* pipeline) {
  assert(!spirv.empty());

  auto start = std::chrono::high_resolution_clock::now();
  spirv_cross::CompilerGLSL glsl(spirv);
  spirv_cross::ShaderResources resources = glsl.get_shader_resources();

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> time = (end - start);
  nei_info("Shader Inspection took {} ms", time.count());


  // attributes - probably not ideal to automate
  if(stage == vk::ShaderStageFlagBits::eVertex) {
    auto gp = pipeline->as<GraphicsPipeline>();

    uint16 attributeMask = 0;
    for(auto& i : resources.stage_inputs) {
      auto location = glsl.get_decoration(i.id, spv::Decoration::DecorationLocation);
      attributeMask |= 1 << location;
    }
    gp->attributeMask = attributeMask;
  }
#if 0
  if (stage == vk::ShaderStageFlagBits::eVertex) {
    auto gp = pipeline->as<GraphicsPipeline>();

    struct Attrib {
      vk::Format format;
      uint location;
      uint size;
    };
    std::vector<Attrib> attribs;

    for(auto &i:resources.stage_inputs) {
      auto location = glsl.get_decoration(i.id, spv::Decoration::DecorationLocation);
      auto type = glsl.get_type(i.base_type_id);
      vk::Format format = attribFormat(type.basetype,type.vecsize);
      attribs.push_back({ format,location,type.width/8*type.vecsize });
    }

    std::sort(attribs.begin(), attribs.end(), [](Attrib const& a, Attrib const&b) {return a.location < b.location; });

    int offset = 0;
    for(auto &a:attribs) {
      gp->addAttribute(0, a.location, a.format, offset);
      offset += a.size;
    }
    if(offset>0)
      gp->addVertexBuffer(0, offset);
  }
#endif

  // attachments
  if(stage == vk::ShaderStageFlagBits::eFragment) {
    auto gp = pipeline->as<GraphicsPipeline>();
    auto& attachments = gp->attachments;
    if(attachments.empty()) {
      for(auto& r : resources.stage_outputs) {
        vk::PipelineColorBlendAttachmentState att;
        att.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::
          ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;;
        attachments.push_back(att);
      }
    }
  }

  // push constants
  for(auto& pc : resources.push_constant_buffers) {
    auto type = glsl.get_type(pc.base_type_id);
    auto size = uint(glsl.get_declared_struct_size(type));
    auto offset = glsl.get_decoration(pc.id, spv::Decoration::DecorationOffset);
    pipeline->addPushConstantRange(size, offset, stage);
  }

  // uniform buffers
  for(auto& u : resources.uniform_buffers) {
    auto type = glsl.get_type(u.base_type_id);
    auto size = uint(glsl.get_declared_struct_size(type));
    auto offset = glsl.get_decoration(u.id, spv::Decoration::DecorationOffset);
    auto set = glsl.get_decoration(u.id, spv::Decoration::DecorationDescriptorSet);
    auto binding = glsl.get_decoration(u.id, spv::Decoration::DecorationBinding);
    auto deset = pipeline->getOrCreateDescriptorSetLayout(set);
    deset->addDescriptor(binding, vk::DescriptorType::eUniformBuffer, stage, 1);
  }

  for(auto& u : resources.sampled_images) {
    auto type = glsl.get_type(u.type_id);
    auto set = glsl.get_decoration(u.id, spv::Decoration::DecorationDescriptorSet);
    auto binding = glsl.get_decoration(u.id, spv::Decoration::DecorationBinding);
    auto count = type.array.empty() ? 1 : type.array[0];
    auto deset = pipeline->getOrCreateDescriptorSetLayout(set);
    deset->addDescriptor(binding, vk::DescriptorType::eCombinedImageSampler, stage, count);
  }

  for(auto& u : resources.storage_images) {
    auto type = glsl.get_type(u.type_id);
    auto set = glsl.get_decoration(u.id, spv::Decoration::DecorationDescriptorSet);
    auto binding = glsl.get_decoration(u.id, spv::Decoration::DecorationBinding);
    auto count = type.array.empty() ? 1 : type.array[0];
    auto deset = pipeline->getOrCreateDescriptorSetLayout(set);
    deset->addDescriptor(binding, vk::DescriptorType::eStorageImage, stage, count);
  }

  for(auto& u : resources.acceleration_structures) {
    auto type = glsl.get_type(u.type_id);
    auto set = glsl.get_decoration(u.id, spv::Decoration::DecorationDescriptorSet);
    auto binding = glsl.get_decoration(u.id, spv::Decoration::DecorationBinding);
    auto count = type.array.empty() ? 1 : type.array[0];
    auto deset = pipeline->getOrCreateDescriptorSetLayout(set);
    deset->addDescriptor(binding, vk::DescriptorType::eAccelerationStructureNV, stage, count);
  }

  for(auto& u : resources.storage_buffers) {
    auto type = glsl.get_type(u.type_id);
    auto set = glsl.get_decoration(u.id, spv::Decoration::DecorationDescriptorSet);
    auto binding = glsl.get_decoration(u.id, spv::Decoration::DecorationBinding);
    auto count = type.array.empty() ? 1 : type.array[0];
    auto deset = pipeline->getOrCreateDescriptorSetLayout(set);
    deset->addDescriptor(binding, vk::DescriptorType::eStorageBuffer, stage, count);
  }
}
