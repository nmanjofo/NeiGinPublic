#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT Texture : public DeviceObject {
  public:
    enum class Usage {
      Sampled,
      RenderBuffer,
      DepthBuffer,
      GBuffer,
      ShadowMap
    };

    Texture(DeviceContext* dc);
    virtual ~Texture();

    vk::Image getImage() const { return image; }

    auto getBaseSize() const { return size; }
    int getLayers() const { return layers; }
    int getMipLevels() const { return levels; }
    auto getFormat() const { return format; }

    void setName(std::string const& name);

    vk::ImageView createView(vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
    vk::ImageView createView(vk::ImageViewType viewType, vk::ImageAspectFlags aspect, int baseLayer, int layers,
                             int baseLevel, int levels);

    operator vk::Image() const { return image; }
    vk::Image operator*() const { return image; }

    vk::ImageSubresourceRange getFullRange();
    void setLayout(CommandBuffer* cmd, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageSubresourceRange range,
                   vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eAllCommands,
                   vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eAllCommands);
    
    void setData(void* data, int layer = 0);
    void setDataAsync(TransferBuffer* tb, void* data, int layer = 0);

    void generateMipMaps(CommandBuffer* cmd=nullptr);
  protected:
    void create(vk::ImageType type, vk::Format format, uvec3 size, int layers, int levels, vk::ImageUsageFlags usage);
    void destroy();
    void resize(uvec3 size);
    vk::ImageUsageFlags usageToFlags(Usage usage);

    vk::ImageLayout layout = vk::ImageLayout::eUndefined; // might not be accurate
    vk::ImageType type;
    vk::ImageUsageFlags usage;
    uvec3 size;
    vk::Format format;
    vk::Image image;
    bool mipmap = false;
    uint layers = 0;
    uint levels = 0;
    Allocation allocation;
    vk::ImageCreateFlags flags;
  };

  class NEIVU_EXPORT Texture2D : public Texture {
  public:
    Texture2D(DeviceContext* dc, uvec2 const& size, vk::Format format, Usage usage = Usage::Sampled,
              bool mipmap = true);

    uvec2 getSize() const { return uvec2(size); }
    void resize(uvec2 const& size);
  };

  class NEIVU_EXPORT TextureCube: public Texture {
  public:
    TextureCube(DeviceContext* dc, uint size, vk::Format format, Usage usage = Usage::Sampled,
      bool mipmap = true);

    uint getSize() const { return size.x; }
    void resize(uint size);
  };
};
