#pragma once

#include "NeiGinBase.h"
#include "Messenger.h"
#include "Gui/Gui.h"
#include "IO/VirtualFileSystem.h"

#include "Assets/AssetManager.h"

#define WAIT_JOBS() Application::getInstance()->getThreadPool()->waitIdle()
#define SEND(X) Application::getInstance()->getMessenger()->send(X)
#define LISTEN(T,...) Application::getInstance()->getMessenger()->addListener<T>(__VA_ARGS__)
#define LISTEN2(T,...) Application::getInstance()->getMessenger()->addListener<T>([&](T const& msg){__VA_ARGS__})

#define NeiApp Application::getInstance()
#define NeiFS Application::getInstance()->getFileSystem()

namespace Nei {
  struct AppMessage {
    enum Type{AppExit} type;
  };

  struct AppFrame {
    int frameId = 0;
    double appTime = 0;
    double appDelta = 0;
    double simTime = 0;
    double simDelta = 0;
  };
  
  class NEIGIN_EXPORT Application {
  public:
    Application(int argc = 0, char** argv = nullptr);
    virtual ~Application();
    Messenger* getMessenger() { return globalMessenger; }
    
    Ptr<Gui>& getGui() { return gui; }
    VirtualFileSystem* getFileSystem() { return fileSystem; }
    AssetManager* getAssetManager() { return assetManager; }

    static Application* getInstance();

    int run();
    void quit() { shouldQuit = true; }
    void setPaused(bool p=true);

  protected:
    Ptr<Messenger> globalMessenger;
    Ptr<Gui> gui;
    Ptr<VirtualFileSystem> fileSystem;
    Ptr<AssetManager> assetManager;

    static Application* globalInstance;

    bool shouldQuit = false;
    int returnCode = 0;
    bool paused = false;

    AppFrame frame;
  };
};
