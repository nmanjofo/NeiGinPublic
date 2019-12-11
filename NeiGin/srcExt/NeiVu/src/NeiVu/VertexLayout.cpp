#include "VertexLayout.h"
#include "Format.h"

using namespace Nei::Vu;

template<typename T>
uint32 combine(uint32 h, T t){
  return uint32(h ^ (std::hash<T>{}(t)+0x9e3779b9 + (h << 6) + (h >> 2)));
}

void VertexLayout::update() {
  stride = 0;
  for(auto &a:attributes) {
    a.size = formatSize(a.format);
    a.offset = stride;
    stride+= a.size;
  }

  hash = 0;
  for(auto &at:attributes) {
    hash = combine(hash, at.size);
    hash = combine(hash, uint(at.format));
    hash = combine(hash, at.offset);
    hash = combine(hash, uint(at.semantic));
  }
}

VertexLayout VertexLayout::pos3Norm3Tc2() {
  VertexLayout layout;
  layout.attributes[0] = Attribute{vk::Format::eR32G32B32Sfloat,AttributeSemantic::Position};
  layout.attributes[1] = Attribute{vk::Format::eR32G32B32Sfloat,AttributeSemantic::Normal};
  layout.attributes[2] = Attribute{vk::Format::eR32G32Sfloat,AttributeSemantic::TexCoord};
  layout.update();
  return layout;
}

VertexLayout VertexLayout::pos3Norm3() {
  VertexLayout layout;
  layout.attributes[0] = Attribute{vk::Format::eR32G32B32Sfloat,AttributeSemantic::Position};
  layout.attributes[1] = Attribute{vk::Format::eR32G32B32Sfloat,AttributeSemantic::Normal};
  layout.update();
  return layout;
}

VertexLayout VertexLayout::pos3Norm3Tc2BoneMat() {
  VertexLayout layout;
  layout.attributes[0] = Attribute{ vk::Format::eR32G32B32Sfloat,AttributeSemantic::Position };
  layout.attributes[1] = Attribute{ vk::Format::eR32G32B32Sfloat,AttributeSemantic::Normal };
  layout.attributes[2] = Attribute{ vk::Format::eR32G32Sfloat,AttributeSemantic::TexCoord };
  layout.attributes[3] = Attribute{ vk::Format::eR8Uint,AttributeSemantic::BoneID };
  layout.attributes[4] = Attribute{ vk::Format::eR8Uint,AttributeSemantic::MaterialID };
  layout.attributes[5] = Attribute{ vk::Format::eR8G8Uint,AttributeSemantic::Padding };
  layout.update();
  return layout;
}

VertexLayout VertexLayout::pos3Norm3Tc2BoneWeightMat() {
  VertexLayout layout;
  layout.attributes[0] = Attribute{ vk::Format::eR32G32B32Sfloat,AttributeSemantic::Position };
  layout.attributes[1] = Attribute{ vk::Format::eR32G32B32Sfloat,AttributeSemantic::Normal };
  layout.attributes[2] = Attribute{ vk::Format::eR32G32Sfloat,AttributeSemantic::TexCoord };
  layout.attributes[3] = Attribute{ vk::Format::eR8G8B8A8Uint,AttributeSemantic::BoneID };
  layout.attributes[4] = Attribute{ vk::Format::eR8G8B8Uint,AttributeSemantic::BoneWight };
  layout.attributes[5] = Attribute{ vk::Format::eR8Uint,AttributeSemantic::MaterialID };
  layout.update();
  return layout;
}
