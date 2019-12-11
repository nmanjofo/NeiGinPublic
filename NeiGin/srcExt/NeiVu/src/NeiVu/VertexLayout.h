#pragma once

#include "NeiVuBase.h"

namespace Nei::Vu {
  struct Vertex_P3N3T2 {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
  };

  struct Vertex_P3N3T2BM {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    uint8 bone;
    uint8 material;
    uint8 pad[2];
  };

  struct Vertex_P3N3T2B4W3M {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    u8vec4 bone;
    u8vec3 weight;
    uint8 material;
  };

  using Vertex_Simple = Vertex_P3N3T2;
  using Vertex_Default = Vertex_P3N3T2BM;
  using Vertex_Skinned = Vertex_P3N3T2B4W3M;

  enum class AttributeSemantic {
    Position,
    Normal,
    TexCoord,
    BoneID,
    BoneWight,
    MaterialID,
    Padding,
    Color,
    Other,
    Unused
  };

  struct Attribute {
    vk::Format format = vk::Format::eUndefined;
    AttributeSemantic semantic = AttributeSemantic::Unused;
    uint size = 0;
    uint offset = 0;
  };

  class NEIVU_EXPORT VertexLayout {
  public:
    bool has(AttributeSemantic semantic);

    void update();
    uint stride = 0;
    Attribute attributes[16];

    uint32 hash = 0;

    bool operator==(VertexLayout const& o) const { return hash == o.hash; }

    static VertexLayout pos3Norm3Tc2();
    static VertexLayout pos3Norm3();
    static VertexLayout pos3Norm3Tc2BoneMat();
    static VertexLayout pos3Norm3Tc2BoneWeightMat();

    static VertexLayout simplelayout() { return pos3Norm3Tc2(); }
    static VertexLayout skinnedLayout() { return pos3Norm3Tc2BoneWeightMat(); }
    static VertexLayout defaultLayout() { return pos3Norm3Tc2BoneMat(); }
  };
};
