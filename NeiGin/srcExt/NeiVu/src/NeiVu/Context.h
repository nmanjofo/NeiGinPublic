#pragma once
#include "NeiVuBase.h"

namespace Nei::Vu {

  struct ContextCreateInfo {
    std::string applicationName = "NeiGin Application";
    int applicationVersion = 0;
    bool validation = true;
  };

  class NEIVU_EXPORT Context : public Object {
  public:
    Context(ContextCreateInfo const& info = ContextCreateInfo());
    virtual ~Context();

    auto getVkInstance() const { return instance; }
    bool isValidationEnabled() const { return validationEnabled; }

    static bool supportsExtension(std::string const& name);
    static bool supportsLayer(std::string const& name);
  protected:
    bool validationEnabled;
    vk::Instance instance;
    vk::DebugReportCallbackEXT debugCallback;
    vk::DispatchLoaderDynamic dynamicDispatch;
  };

};
