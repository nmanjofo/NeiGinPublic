#pragma once

#include "Material.h"

namespace Nei{
  struct PBRMaterialData {
    int albedoTexIndex;
    int pad[3];
  };

  class NEIGIN_EXPORT PBRMaterial : public Material {
  public:
    PBRMaterial();
    virtual ~PBRMaterial();

    fs::path albedoTexturePath;
    Ptr<Texture2D> albedoTexture;

    fs::path metallicTexturePath;
    Ptr<Texture2D> metallicTexture;

    fs::path roughnessTexturePath;
    Ptr<Texture2D> roughnessTexture;

    fs::path normalTexturePath;
    Ptr<Texture2D> normalTexture;

    fs::path emissiveTexturePath;
    Ptr<Texture2D> emissiveTexture;
  protected:
  };
};


