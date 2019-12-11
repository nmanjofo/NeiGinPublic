#pragma once

#include "NeiGinBase.h"
#include "Image.h"

namespace Nei {
  class NEIGIN_EXPORT ImageLoader : public Object {
  public:
    ImageLoader(){}
    virtual ~ImageLoader(){}

    Ptr<Image> loadImage(void* data, uint size, std::string const& name="");
    Ptr<Image> loadImage(fs::path const& path);
    Ptr<Image> dummyImage(uvec2 const& size = { 512,512 });

    void setGenerateDummy(bool gen) { generateDummy = gen; }

  protected:
    bool generateDummy = true;
  };
};
