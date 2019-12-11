#pragma once

#include "NeiGinBase.h"

namespace Nei{
  class NEIGIN_EXPORT VirtualFileSystem: public Object{
  public:
    VirtualFileSystem();
    virtual ~VirtualFileSystem();

    void mount(std::filesystem::path const& path);

    std::filesystem::path resolve(std::filesystem::path const& path, bool warn=true);

  protected:
    std::vector<std::filesystem::path> mountedPaths;
    
  };
};


