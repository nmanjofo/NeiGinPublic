#include "FxLoader.h"

#include "DeviceContext.h"
#include "ComputePipeline.h"
#include "GraphicsPipeline.h"
#include "RaytracingPipeline.h"
#include "Shader.h"

#include <sstream>
#include <fstream>

using namespace Nei;
using namespace Vu;


bool parseBool(std::string const& s) {
  if (s == "enable" || s == "true") return true;
  if (s == "disable" || s == "false") return false;
  nei_error("Can't convert {} to bool", s);
  return false;
}

float praseFloat(std::string const& s) {
  try {
    return std::stof(s);
  }
  catch (...) {
    nei_error("Can't convert {} to float", s);
    return 0;
  }
}

int parseInt(std::string const& s) {
  try {
    return std::stoi(s);
  }
  catch (...) {
    nei_error("Can't convert {} to int", s);
    return 0;
  }
}

vk::ColorComponentFlags parseColorMask(std::string const& s) {
  vk::ColorComponentFlags ret;
  if (s.find("r") != std::string::npos) ret |= vk::ColorComponentFlagBits::eR;
  if (s.find("g") != std::string::npos) ret |= vk::ColorComponentFlagBits::eG;
  if (s.find("b") != std::string::npos) ret |= vk::ColorComponentFlagBits::eB;
  if (s.find("a") != std::string::npos) ret |= vk::ColorComponentFlagBits::eA;
  return ret;
}

vk::PrimitiveTopology parseTopology(std::string const& s) {
  if (s == "lineList") return vk::PrimitiveTopology::eLineList;
  if (s == "lineStrip") return vk::PrimitiveTopology::eLineStrip;
  if (s == "pointList") return vk::PrimitiveTopology::ePointList;
  if (s == "triangleFan") return vk::PrimitiveTopology::eTriangleFan;
  if (s == "triangleList") return vk::PrimitiveTopology::eTriangleList;
  if (s == "triangleStrip") return vk::PrimitiveTopology::eTriangleStrip;
  nei_error("Can't convert {} to topology", s);
  return vk::PrimitiveTopology::eTriangleList;
}

vk::CullModeFlagBits parseCulling(std::string const& s) {
  if (s == "none") return vk::CullModeFlagBits::eNone;
  if (s == "back") return vk::CullModeFlagBits::eBack;
  if (s == "front") return vk::CullModeFlagBits::eFront;
  if (s == "frontAndBack") return vk::CullModeFlagBits::eFrontAndBack;
  return vk::CullModeFlagBits::eNone;
}


FxLoader::FxLoader(DeviceContext* deviceContext): DeviceObject(deviceContext) { }
FxLoader::~FxLoader() {}

Ptr<Pipeline> FxLoader::loadFx(std::string const& fx, std::string const& fxName) const {
  if(fxName.empty()) return nullptr;

  if (fx.find("#comp") != std::string::npos) {
    return parseCompute(fx, fxName).as<Pipeline>();
  }

  if (fx.find("#vert") != std::string::npos) {
    return parseGraphics(fx, fxName).as<Pipeline>();
  }

  if (fx.find("#rgen") != std::string::npos) {
    return parseRaytracing(fx, fxName).as<Pipeline>();
  }
  nei_error("Invalid FX! {}",fxName);
  return nullptr;
}

Ptr<Pipeline> FxLoader::loadFxFile(fs::path const& fxFile) const {
  std::ifstream stream(fxFile);
  std::stringstream fx;
  fx << stream.rdbuf();
  return loadFx(fx.str(), fxFile.filename().string());
}

Ptr<ComputePipeline> FxLoader::parseCompute(std::string const& fx, std::string const& fxName) const {
  std::stringstream stream(fx);
  std::string line;
  int lines = 0;

  Ptr pipeline = new ComputePipeline(deviceContext);

  std::string version = defaultVersion;

  std::stringstream shared;
  std::stringstream comp;
  std::stringstream* current = &shared;

  while (std::getline(stream, line)) {
    lines++;
    if (line._Starts_with("#version")) {
      version = line;
    } else if (line == "#comp") {
      current = &comp;
      *current << version << "\n";
      *current << shared.str();
    } else {
      *current << line << "\n";
    }
  }

  pipeline->addShader(new Shader(deviceContext, comp.str(), vk::ShaderStageFlagBits::eCompute,fxName));
  pipeline->create();
  return pipeline;
}

Ptr<GraphicsPipeline> FxLoader::parseGraphics(std::string const& fx, std::string const& fxName) const {
  Ptr pipeline = new GraphicsPipeline(deviceContext);

  std::stringstream stream(fx);
  std::string line;
  int lines = 0;

  std::string version = defaultVersion;

  vk::ShaderStageFlagBits stage;
  std::stringstream shared;
  std::stringstream src;
  std::stringstream* current = &shared;

  auto finishStage = [&]() {
    if (current == &src)
      pipeline->addShader(new Shader(deviceContext, src.str(), stage,fxName));
  };

  auto newStage = [&](vk::ShaderStageFlagBits newStageFlag) {
    finishStage();
    current = &src;
    src = std::stringstream();
    src << version << "\n";
    src << shared.str();
    stage = newStageFlag;
  };

  while (std::getline(stream, line)) {
    lines++;
    if (line._Starts_with("#version")) {
      version = line;
    } else if (parsePipeline(line, pipeline)) { } else if (line == "#vert") {
      newStage(vk::ShaderStageFlagBits::eVertex);
    } else if (line == "#tesc") {
      newStage(vk::ShaderStageFlagBits::eTessellationControl);
    } else if (line == "#tese") {
      newStage(vk::ShaderStageFlagBits::eTessellationEvaluation);
    } else if (line == "#geo") {
      newStage(vk::ShaderStageFlagBits::eGeometry);
    } else if (line == "#frag") {
      newStage(vk::ShaderStageFlagBits::eFragment);
    } else {
      *current << line << "\n";
    }
  }
  finishStage();
  pipeline->createLayout();

  return pipeline;
}

Ptr<RaytracingPipeline> FxLoader::parseRaytracing(std::string const& fx, std::string const& fxName) const {
  Ptr pipeline = new RaytracingPipeline(deviceContext);

  std::stringstream stream(fx);
  std::string line;
  int lines = 0;

  std::string version = defaultVersion;

  vk::ShaderStageFlagBits stage;
  std::stringstream shared;
  std::stringstream src;
  std::stringstream* current = &shared;

  Ptr<Shader> intersection;
  Ptr<Shader> any;
  Ptr<Shader> closest;

  auto finishStage = [&]() {
    if (current != &src) return;
    if (stage == vk::ShaderStageFlagBits::eRaygenNV)
      pipeline->addRayGenShader(new Shader(deviceContext, src.str(), stage));
    if (stage == vk::ShaderStageFlagBits::eMissNV)
      pipeline->addMissShader(new Shader(deviceContext, src.str(), stage));
    if (stage == vk::ShaderStageFlagBits::eIntersectionNV)
      intersection = new Shader(deviceContext, src.str(), stage);
    if (stage == vk::ShaderStageFlagBits::eClosestHitNV)
      closest = new Shader(deviceContext, src.str(), stage);
    if (stage == vk::ShaderStageFlagBits::eAnyHitNV)
      any = new Shader(deviceContext, src.str(), stage);
    current = nullptr;
  };

  auto newStage = [&](vk::ShaderStageFlagBits newStageFlag) {
    finishStage();
    current = &src;
    src = std::stringstream();
    src << version << "\n";
    src << shared.str();
    stage = newStageFlag;
  };

  auto finishHitgroup = [&]() {
    if (intersection || any || closest) {
      pipeline->addHitShader(closest, any, intersection);
    }
  };

  auto newHitgroup = [&]() {
    finishHitgroup();
    intersection = nullptr;
    any = nullptr;
    closest = nullptr;
  };

  while (std::getline(stream, line)) {
    lines++;
    if (line._Starts_with("#version")) {
      version = line;
    } else if (line._Starts_with("#depth")) {
      int d = parseInt(line.substr(7));
      pipeline->setRecursionDepth(d);
    } else if (line == "#rgen") {
      newStage(vk::ShaderStageFlagBits::eRaygenNV);
    } else if (line == "#rmiss") {
      newStage(vk::ShaderStageFlagBits::eMissNV);
    } else if (line == "#rahit") {
      newStage(vk::ShaderStageFlagBits::eAnyHitNV);
    } else if (line == "#rchit") {
      newStage(vk::ShaderStageFlagBits::eClosestHitNV);
    } else if (line == "#rint") {
      newStage(vk::ShaderStageFlagBits::eIntersectionNV);
    } else if (line == "#rcall") {
      newStage(vk::ShaderStageFlagBits::eCallableNV);
    } else if (line == "#hitgroup") {
      finishStage();
      newHitgroup();
    } else {
      *current << line << "\n";
    }
  }

  finishStage();
  finishHitgroup();

  pipeline->createLayout();
  pipeline->create();

  return pipeline;
}

bool FxLoader::parsePipeline(std::string const& line, GraphicsPipeline* pipe) const {
  std::string tag;
  std::string value;

  std::stringstream linestream(line);
  linestream >> tag;
  linestream >> value;
  // depthstencil
  if (tag == "#depthTestEnable") {
    pipe->depthStencil.depthTestEnable = parseBool(value);
  }
    // rasterisation
  else if (tag == "#depthBiasEnable") {
    pipe->rasterisation.depthBiasEnable = parseBool(value);
  } else if (tag == "#depthBiasConstantFactor") {
    pipe->rasterisation.depthBiasConstantFactor = praseFloat(value);
  } else if (tag == "#depthBiasSlopeFactor") {
    pipe->rasterisation.depthBiasSlopeFactor = praseFloat(value);
  } else if (tag == "#wireframe") {
    pipe->rasterisation.polygonMode = vk::PolygonMode::eLine;
  }else if(tag == "#cull") {
    pipe->rasterisation.cullMode = parseCulling(value);
    pipe->rasterisation.frontFace = vk::FrontFace::eCounterClockwise;
  }
    // attachments
  else if (tag == "#attachment") {
    vk::PipelineColorBlendAttachmentState att;
    att.colorWriteMask = parseColorMask(value);
    pipe->attachments.push_back(att);
  }
    // primitive
  else if (tag == "#patches") {
    pipe->tessellation.patchControlPoints = parseInt(value);
    pipe->primitive.topology = vk::PrimitiveTopology::ePatchList;

  } else if (tag == "#topology") {
    pipe->primitive.topology = parseTopology(value);

  } else {
    return false;
  }

  return true;
}
