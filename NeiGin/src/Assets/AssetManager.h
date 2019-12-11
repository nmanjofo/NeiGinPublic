#pragma once

#include "NeiGinBase.h"
#include "NeiVu/DeviceObject.h"

namespace Nei {
  class NEIGIN_EXPORT AssetManager : public Vu::DeviceObject {
  public:
    AssetManager(DeviceContext* dc);
    virtual ~AssetManager();

    Ptr<Texture2D> loadTexture2D(fs::path const& path, Ptr<TransferBuffer> tb = nullptr);
    Ptr<Texture2D> loadTexture2D(void* data, uint size, std::string const& name, Ptr<TransferBuffer> tb = nullptr);

    Ptr<TextureCube> loadTextureCube(fs::path path[6], Ptr<TransferBuffer> tb = nullptr);

    Ptr<Pipeline> loadFx(fs::path const& path);

    Ptr<Model> loadModel(fs::path const& path);

    Ptr<Texture2D> createDummy(uvec2 const& size, Ptr<TransferBuffer> tb = nullptr);
    Ptr<Texture2D> createPixelTexture(vec4 const& color, Ptr<TransferBuffer> tb = nullptr);

  protected:
    Ptr<ImageLoader> imageLoader;

    std::map<std::string, Ptr<Image>> images;
    std::map<std::string, Ptr<Texture2D>> textures;
  };
};
