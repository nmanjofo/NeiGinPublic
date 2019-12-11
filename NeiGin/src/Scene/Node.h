#pragma once

#include "NeiGinBase.h"
#include "Math/Transform.h"
#include "Math/AABB.h"

namespace Nei {
  class NEIGIN_EXPORT Node : public Object {
  public:

    enum Tag : uint64 {
      None = 0,
      Enabled = bit( 0 ),
      Transform = bit( 1 ),
      All = ~0u
    };

    enum class NodeType : uint{
      Special,
      Node,
      SimpleModel,
      Model,
      ModelInstance,
      LodSwitch,
      Camera,
      Light,
      Mesh
    };

    enum class TransformType {
      None,
      Local,
      World
    };

    Node();
    virtual ~Node();
    static NodeType staticType(){return NodeType::Node;}

    void setName(std::string const& name){this->name=name;}
    std::string const& getName()const{return name;}

    bool hasTags(uint64 tags) const { return tags & tag; }
    void setTags(uint64 tags) { tag |= tags; }
    void unsetTags(uint64 tags) { tag &= ~tags; }

    auto getType() const { return type; }
    const char* getTypeName() const;

    void setMask(uint64 mask_) { mask = mask_; }
    auto getMask() const { return mask; }

    mat4 const& getLocalMatrix() const { return localMatrix; }
    void setLocalMatrix(mat4 const& mat);

    mat4 const& getWorldMatrix() const { return worldMatrix; }

    auto& getAABB() const { return aabb; }
    virtual void updateAABB();

    void add(Ptr<Node> const& node);
    void remove(Ptr<Node> const& node);
    void setParent(Ptr<Node> const& node);
    void removeParent();

    auto& getChildren() { return children; }

    void updateTransform();

    bool isDirty() const {return transformDirty;}
    void makeDirty() {transformDirty = true;}
    bool wasDirty() const {return transformUpdated;}
  protected:

    NodeType type = NodeType::Node;
    uint64 tag = Enabled;
    uint64 mask = 0;
    Node* parent = nullptr;
    std::vector<Ptr<Node>> children;
    AABB aabb;
    std::string name;

    bool transformDirty = true;
    bool transformUpdated = true;
    TransformType transformType = TransformType::None;
    mat4 localMatrix;
    mat4 worldMatrix;

  };
};
