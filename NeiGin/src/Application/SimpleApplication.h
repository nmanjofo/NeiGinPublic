#pragma once

#include "NeiGinBase.h"
#include "Application.h"
#include "Window.h"
#include "NeiVu/DeviceContext.h"
#include "NeiVu/Swapchain.h"

namespace Nei {
  class NEIGIN_EXPORT SimpleApplication : public Application {
  public:
    struct Options {
      std::vector<const char*> instanceExtensions;
      std::vector<const char*> deviceExtensions;
      bool vsync = false;
      bool validation = true;
      WindowCreateParams window;
    };

    SimpleApplication(void*);
    SimpleApplication(Options const& options = Options());
    virtual ~SimpleApplication();

    void init(Options const& options = Options());

    virtual void update(AppFrame const& frame) {
      if (updateCallback)updateCallback(frame);
    }

    virtual void draw() {
      if (drawCallback)drawCallback();
    }

    virtual void resize(ivec2 size) {
      if (resizeCallback)resizeCallback(size);
    }

    virtual void onMessage(WindowMessage const& msg) {
      if (windowMessageCallback)windowMessageCallback(msg);
    }

    std::function<void(WindowMessage const&)> windowMessageCallback;
    std::function<void(AppFrame const&)> updateCallback;
    std::function<void()> drawCallback;
    std::function<void(uvec2)> resizeCallback;

    Ptr<Window> window;
    Ptr<Vu::Swapchain> swapchain;
    Ptr<Vu::Context> context;
    Ptr<Vu::DeviceContext> deviceContext;

    Ptr<Camera> camera;
    Ptr<OrbitManipulator> manipulator;

    vk::Instance instance;
    vk::Device device;
    vk::PhysicalDevice physicalDevice;
  };
};
