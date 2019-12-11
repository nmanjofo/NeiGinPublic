#pragma once

#define NEIVU_VERSION 0

namespace Nei::Vu {
  class Context;
  class CommandBuffer;
  class CommandPool;
  class DeviceContext;
  class DeviceObject;
  class Buffer;
  class Texture;
  class Texture1D;
  class Texture2D;
  class Texture2DArray;
  class Texture3D;
  class TextureCube;
  class TextureCubeArray;
  class Pipeline;
  class GraphicsPipeline;
  class ComputePipeline;
  class RaytracingPipeline;
  class DescriptorPool;
  class DescriptorSetLayout;
  class DescriptorSet;
  class SamplerManager;
  class Shader;
  class FxLoader;
  class Swapchain;
  class RenderPass;
  class Framebuffer;
  class ShaderBindingTable;
  class AccelerationStructure;
  class VertexLayout;
  class TransferBuffer;
  class MemoryManager;
  struct MemoryBlock;
  class GBuffer;
  class Fence;
};

#include "NeiCore.h"

#include "Export.h"
#include "Scope.h"

#pragma warning( push )
#pragma warning( disable : 26495 )
#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.hpp> // this includes Windows.h cancer
#pragma warning( pop )

#include "tracy/TracyVulkan.hpp"
#define ProfileGPU(cmd,name) TracyVkZone(cmd->getDeviceContext()->getTracyContext(),**cmd,name);Profile(name);
#define ProfileCollect(cmd) TracyVkCollect(cmd->getDeviceContext()->getTracyContext(), **cmd);

#include "Enums.h"
#include "Allocation.h"
