#include "SimpleApplication.h"
#include "Scene/Camera.h"
#include "Scene/OrbitManipulator.h"
#include "Assets/AssetManager.h"
#include "NeiVu/Context.h"

using namespace Nei;
using namespace Vu;

SimpleApplication::SimpleApplication(void*) {

}

SimpleApplication::SimpleApplication(Options const& options) {
  init(options);
}

SimpleApplication::~SimpleApplication() {}

void SimpleApplication::init(Options const& options) {
  ContextCreateInfo cci;
  cci.validation = options.validation;
  context = new Context(cci);

  DeviceContext::CreateInfo dci;
  dci.context = context;
  dci.extensions = options.deviceExtensions;

  deviceContext = new DeviceContext(dci);

  window = new Window(options.window);

  SwapchainCreateInfo sci;
  sci.deviceContext = deviceContext;
  sci.windowHandle = window->getNativeHandle();
  sci.vsync = options.vsync;
  swapchain = new Swapchain(sci);
  device = deviceContext->getVkDevice();
  physicalDevice = deviceContext->getVkPhysicalDevice();
  instance = context->getVkInstance();

  gui->setCurrent(window);

  camera = new Camera();
  manipulator = new OrbitManipulator(camera);
  assetManager = new AssetManager(deviceContext);

  globalMessenger->addListener<WindowMessage>([&](WindowMessage const& msg) {
    {
      switch (msg.type) {
      case WindowMessage::MessageType::WindowResize: {
        device.waitIdle();
        swapchain->resize();

        auto fovy = camera->getFov().y;
        camera->setFov(fovy, msg.size);

        resize(msg.size);
        break;
      }
      case WindowMessage::MessageType::WindowClose:
        shouldQuit = true;
        break;
      case WindowMessage::MessageType::Key:
        if (msg.key == Key::Escape)
          shouldQuit = true;
        break;
      }
      onMessage(msg);
    }
    });

  globalMessenger->addListener<AppFrame>([&](AppFrame const& msg) {
    FrameMark;
    {
      Profile("Update");
      manipulator->update();
      update(msg);
    }
    {
      Profile("Render");
      draw();
    }
    });

  globalMessenger->addListener<AppMessage>([&](AppMessage const& msg) {
    switch (msg.type) {
    case AppMessage::AppExit:
      deviceContext->getVkDevice().waitIdle();
      break;
    }
    });
};
