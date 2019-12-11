#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT FxLoader : public DeviceObject {
  public:
    const std::string defaultVersion = "#version 460";

    FxLoader(DeviceContext* deviceContext);
    virtual ~FxLoader();

    Ptr<Pipeline> loadFx(std::string const& fx, std::string const& fxName = "inlineFx") const;
    Ptr<Pipeline> loadFxFile(fs::path const& fxFile) const;

  protected:
    Ptr<ComputePipeline> parseCompute(std::string const& fx, std::string const& fxName) const;
    Ptr<GraphicsPipeline> parseGraphics(std::string const& fx, std::string const& fxName) const;
    Ptr<RaytracingPipeline> parseRaytracing(std::string const& fx, std::string const& fxName) const;

    bool parsePipeline(std::string const& line, GraphicsPipeline* pipe) const;
  };
};
