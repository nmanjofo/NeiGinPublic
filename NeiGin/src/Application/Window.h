#pragma once

#include "NeiGinBase.h"
#include "Messenger.h"
#include "Input.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace Nei {
  struct NEIGIN_EXPORT WindowMessage{
    enum class MessageType { WindowResize, WindowMove, WindowClose, MouseMove, MouseButton, MouseWheel, Key };

    WindowMessage(Window* w, MessageType t): window(w), type(t) {}

    MessageType type;
    Ptr<Window> window;
	  ivec2 delta;
    ivec2 pos;
    ivec2 size;
    int button;
    Nei::Key key;
	  int mods;
    bool press;
  };

  enum FullscreenType {
    FULLSCREEN_BORDERLESS,
    FULLSCREEN_WINDOW
  };

  struct NEIGIN_EXPORT WindowCreateParams {
    std::string title = "New Window";
    ivec2 size = {1024, 768};
    int display = 0;
    bool maximized = false;
    bool borderless = false;
    bool resizable = true;
    bool fullscreen = false;
    FullscreenType fullscreenType = FULLSCREEN_BORDERLESS;
  };

  class NEIGIN_EXPORT Window : public Object {
  public:
    Window(WindowCreateParams const& params = WindowCreateParams());
    virtual ~Window();

    void setSize(ivec2 const& size);
    ivec2 getSize() const;
    void setPosition(ivec2 const& pos);
    ivec2 getPosition() const;
    void close();
    void setFullscreen(bool fullscreen);
    bool getFullscreen() const;
    void* getNativeHandle() const;
    bool isClosed() const;
    void setTitle(std::string const&title);

    GLFWwindow* getGlfwWindow()const{return glfwWindow;}

    bool isMouseCaptured();
    bool isKeyboardCaputred();

    void setResizable(bool resizable);
    protected:
    void createGlfwWindow();
    void destroyGlfwWindow();
    void recenter();
    GLFWmonitor* getGlfwMonitor(int index);

    WindowCreateParams params;
    GLFWwindow* glfwWindow = nullptr;
  };

}
