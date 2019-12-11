#include "Application.h"
#include "Assets/AssetManager.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


using namespace Nei;

Application* Application::globalInstance = nullptr;

Application::Application(int argc, char** argv) {
  assert(globalInstance == nullptr);
  globalInstance = this;
  globalMessenger = new Messenger;
  gui = Gui::getInstance();
  fileSystem = new VirtualFileSystem;
  glfwInit();
}

Application::~Application() {
  glfwTerminate();
}

Application* Application::getInstance() {
  assert(globalInstance != nullptr);
  return globalInstance;
}

int Application::run() {
  auto start = std::chrono::high_resolution_clock::now();
  auto lastFrame = start;

  try {
    while(!shouldQuit) {
      /* Poll for and process events */
      glfwPollEvents();

      auto now = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> dt = now - lastFrame;
      std::chrono::duration<double> time = now - start;
      lastFrame = now;

      frame.frameId++;
      frame.appTime = time.count();
      frame.appDelta = dt.count();

      if(!paused) {
        frame.simTime += dt.count();
        frame.simDelta = dt.count();
      } else {
        frame.simDelta = 0;
      }

      SEND(frame);

      //shouldQuit |= BAGIRA_WINDOW_MANAGER->GetWindows().IsEmpty();
    }
  } catch(std::system_error const& er) {
    nei_fatal("Fatal exception: {}",er.what());
  }
  SEND(AppMessage{AppMessage::AppExit});
  return returnCode;
}

void Application::setPaused(bool p) {
  paused = p;
}
