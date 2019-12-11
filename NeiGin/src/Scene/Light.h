#pragma once

#include "Node.h"

namespace Nei {
  class NEIGIN_EXPORT Light : public Node {
  public:
    enum class LightType {
      point
    };

    Light();
    virtual ~Light();
    static NodeType staticType(){return NodeType::Light;}
    
    void updateAABB() override;

    LightType getLightType() const { return lightType; }
    void setLightType(LightType type) { lightType = type; }

    auto& getColor() const { return color; }
    void setColor(vec4 const& c) { color = c; }

    void setRadius(float r) { radius = r; }
    float getRadius() const { return radius; }
  protected:
    LightType lightType = LightType::point;
    vec4 color;
    float radius = 0;
  };
};
