#pragma once

#include "NeiVuBase.h"

namespace Nei::Vu {
  enum class SamplerType;

  const int MissingQueue = -1;
  const int DefaultQueue = -2;

  class NEIVU_EXPORT DeviceContext : public Object {
  public:
    struct CreateInfo {
      Context* context = nullptr;
      uint32 deviceId = 0;
      std::vector<const char*> extensions;
      std::vector<const char*> layers;
    };

    DeviceContext(CreateInfo const& createInfo);
    virtual ~DeviceContext();

    bool supportsExtension(std::string const& name) const ;
    bool supportsLayer(std::string const& name) const ;
    bool supportsImageFormat(vk::Format format, vk::FormatFeatureFlags usage);
    bool supportsDepthFormat(vk::Format format);

    Ptr<Context> const& getContext() const  { return context; }
    vk::Device getVkDevice() const  { return device; }
    vk::PhysicalDevice getVkPhysicalDevice() const  { return physicalDevice; }

    int getMainQueueIndex() const   { return mainQueueIndex; }
    int getTransferQueueIndex() const   { return transferQueueIndex; }

    vk::Queue getMainQueue() const  { return mainQueue; }
    vk::Queue getTransferQueue() const  { return transferQueue; }

    vk::Queue getPresentQueue() const  { return presentQueue; }
    vk::Queue getQueue(int index) const  ;

    vk::CommandPool getCommandPool(int queueIndex) ;
    vk::PipelineCache getPipelineCache() const  { return pipelineCache; }

    Ptr<MemoryManager> getMemoryManager() const ;

    vk::DispatchLoaderDynamic const& getDispatch()  { return dispatch; }

    void submit(int queueIndex, std::vector<Ptr<CommandBuffer>> const& buffers, Ptr<Fence> const&fence = nullptr,
                std::vector<vk::Semaphore> const& wait = {},
                std::vector<vk::Semaphore> const& signal = {},
                std::vector<vk::PipelineStageFlags> const& stages = {}) ;

     CommandBuffer* getSingleUseCommandBuffer() ;

    vk::Sampler getSampler(SamplerType type);

    FxLoader* getFxLoader() const  {return fxLoader;}
    Ptr<GraphicsPipeline> loadFx(std::string const&fx) const ;
    Ptr<GraphicsPipeline> loadFx(fs::path const& fx) const ;
    Ptr<ComputePipeline> loadComp(fs::path const& fx) const ;

    void wait() ;

    tracy::VkCtx* getTracyContext()  { return tracyContext; }

    bool validation() const;

  protected:
    Ptr<Context> context;
    vk::Device device;
    vk::PhysicalDevice physicalDevice;

    Ptr<MemoryManager> memoryManager;

    vk::Queue mainQueue;
    int mainQueueIndex = MissingQueue;
    vk::Queue computeQueue;
    int computeQueueIndex = MissingQueue;
    vk::Queue transferQueue;
    int transferQueueIndex = MissingQueue;

    vk::Queue presentQueue;
    int presentQueueIndex = MissingQueue;

    std::map<uint64, vk::CommandPool> commandPoolMap;
    vk::PipelineCache pipelineCache;

    vk::DispatchLoaderDynamic dispatch;

    Ptr<CommandBuffer> singleUseCommandBuffer;
    Ptr<SamplerManager> samplerManager;
    Ptr<FxLoader> fxLoader;

    tracy::VkCtx* tracyContext=nullptr;

  };
};
