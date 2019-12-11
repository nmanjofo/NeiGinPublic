#include "Window.h"
#include "Application.h"
#include "Input.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

using namespace Nei;

static std::map<int, Nei::Key> keyMap = {
  {GLFW_KEY_SPACE, Nei::Key::Space},
  {GLFW_KEY_APOSTROPHE, Nei::Key::Apostrophe},
  {GLFW_KEY_COMMA, Nei::Key::Comma},
  {GLFW_KEY_MINUS, Nei::Key::Minus},
  {GLFW_KEY_PERIOD, Nei::Key::Period},
  {GLFW_KEY_SLASH, Nei::Key::Slash},
  {GLFW_KEY_0, Nei::Key::N0},
  {GLFW_KEY_1, Nei::Key::N1},
  {GLFW_KEY_2, Nei::Key::N2},
  {GLFW_KEY_3, Nei::Key::N3},
  {GLFW_KEY_4, Nei::Key::N4},
  {GLFW_KEY_5, Nei::Key::N5},
  {GLFW_KEY_6, Nei::Key::N6},
  {GLFW_KEY_7, Nei::Key::N7},
  {GLFW_KEY_8, Nei::Key::N8},
  {GLFW_KEY_9, Nei::Key::N9},
  {GLFW_KEY_SEMICOLON, Nei::Key::Semicolon},
  {GLFW_KEY_EQUAL, Nei::Key::Equal},
  {GLFW_KEY_A, Nei::Key::A},
  {GLFW_KEY_B, Nei::Key::B},
  {GLFW_KEY_C, Nei::Key::C},
  {GLFW_KEY_D, Nei::Key::D},
  {GLFW_KEY_E, Nei::Key::E},
  {GLFW_KEY_F, Nei::Key::F},
  {GLFW_KEY_G, Nei::Key::G},
  {GLFW_KEY_H, Nei::Key::H},
  {GLFW_KEY_I, Nei::Key::I},
  {GLFW_KEY_J, Nei::Key::J},
  {GLFW_KEY_K, Nei::Key::K},
  {GLFW_KEY_L, Nei::Key::L},
  {GLFW_KEY_M, Nei::Key::M},
  {GLFW_KEY_N, Nei::Key::N},
  {GLFW_KEY_O, Nei::Key::O},
  {GLFW_KEY_P, Nei::Key::P},
  {GLFW_KEY_Q, Nei::Key::Q},
  {GLFW_KEY_R, Nei::Key::R},
  {GLFW_KEY_S, Nei::Key::S},
  {GLFW_KEY_T, Nei::Key::T},
  {GLFW_KEY_U, Nei::Key::U},
  {GLFW_KEY_V, Nei::Key::V},
  {GLFW_KEY_W, Nei::Key::W},
  {GLFW_KEY_X, Nei::Key::X},
  {GLFW_KEY_Y, Nei::Key::Y},
  {GLFW_KEY_Z, Nei::Key::Z},
  {GLFW_KEY_LEFT_BRACKET, Nei::Key::LeftBracket},
  {GLFW_KEY_BACKSLASH, Nei::Key::Backslash},
  {GLFW_KEY_RIGHT_BRACKET, Nei::Key::RightBracket,},
  {GLFW_KEY_GRAVE_ACCENT, Nei::Key::GraveAccent},
  {GLFW_KEY_WORLD_1, Nei::Key::World1},
  {GLFW_KEY_WORLD_2, Nei::Key::World2},
  {GLFW_KEY_ESCAPE, Nei::Key::Escape},
  {GLFW_KEY_ENTER, Nei::Key::Enter},
  {GLFW_KEY_TAB, Nei::Key::Tab},
  {GLFW_KEY_BACKSPACE, Nei::Key::Backspace},
  {GLFW_KEY_INSERT, Nei::Key::Insert},
  {GLFW_KEY_DELETE, Nei::Key::Delete},
  {GLFW_KEY_RIGHT, Nei::Key::Right},
  {GLFW_KEY_LEFT, Nei::Key::Left},
  {GLFW_KEY_DOWN, Nei::Key::Down},
  {GLFW_KEY_UP, Nei::Key::Up},
  {GLFW_KEY_PAGE_UP, Nei::Key::PageUp},
  {GLFW_KEY_PAGE_DOWN, Nei::Key::PageDown},
  {GLFW_KEY_HOME, Nei::Key::Home},
  {GLFW_KEY_END, Nei::Key::End},
  {GLFW_KEY_CAPS_LOCK, Nei::Key::CapsLock},
  {GLFW_KEY_SCROLL_LOCK, Nei::Key::ScrollLock},
  {GLFW_KEY_NUM_LOCK, Nei::Key::NumLock},
  {GLFW_KEY_PRINT_SCREEN, Nei::Key::PrintScreen},
  {GLFW_KEY_PAUSE, Nei::Key::Pause},
  {GLFW_KEY_F1, Nei::Key::F1},
  {GLFW_KEY_F2, Nei::Key::F2},
  {GLFW_KEY_F3, Nei::Key::F3},
  {GLFW_KEY_F4, Nei::Key::F4},
  {GLFW_KEY_F5, Nei::Key::F5},
  {GLFW_KEY_F6, Nei::Key::F6},
  {GLFW_KEY_F7, Nei::Key::F7},
  {GLFW_KEY_F8, Nei::Key::F8},
  {GLFW_KEY_F9, Nei::Key::F9},
  {GLFW_KEY_F10, Nei::Key::F10},
  {GLFW_KEY_F11, Nei::Key::F11},
  {GLFW_KEY_F12, Nei::Key::F12},
  {GLFW_KEY_F13, Nei::Key::F13},
  {GLFW_KEY_F14, Nei::Key::F14},
  {GLFW_KEY_F15, Nei::Key::F15},
  {GLFW_KEY_F16, Nei::Key::F16},
  {GLFW_KEY_F17, Nei::Key::F17},
  {GLFW_KEY_F18, Nei::Key::F18},
  {GLFW_KEY_F19, Nei::Key::F19},
  {GLFW_KEY_F20, Nei::Key::F20},
  {GLFW_KEY_F21, Nei::Key::F21},
  {GLFW_KEY_F22, Nei::Key::F22},
  {GLFW_KEY_F23, Nei::Key::F23},
  {GLFW_KEY_F24, Nei::Key::F24},
  {GLFW_KEY_F25, Nei::Key::F25},
  {GLFW_KEY_KP_0, Nei::Key::Kp0},
  {GLFW_KEY_KP_1, Nei::Key::Kp1},
  {GLFW_KEY_KP_2, Nei::Key::Kp2},
  {GLFW_KEY_KP_3, Nei::Key::Kp3},
  {GLFW_KEY_KP_4, Nei::Key::Kp4},
  {GLFW_KEY_KP_5, Nei::Key::Kp5},
  {GLFW_KEY_KP_6, Nei::Key::Kp6},
  {GLFW_KEY_KP_7, Nei::Key::Kp7},
  {GLFW_KEY_KP_8, Nei::Key::Kp8},
  {GLFW_KEY_KP_9, Nei::Key::Kp9},
  {GLFW_KEY_KP_DECIMAL, Nei::Key::KpDecimal},
  {GLFW_KEY_KP_DIVIDE, Nei::Key::KpDivide},
  {GLFW_KEY_KP_MULTIPLY, Nei::Key::KpMultiply},
  {GLFW_KEY_KP_SUBTRACT, Nei::Key::KpSubtract},
  {GLFW_KEY_KP_ADD, Nei::Key::KpAdd},
  {GLFW_KEY_KP_ENTER, Nei::Key::KpEnter},
  {GLFW_KEY_KP_EQUAL, Nei::Key::KpEqual},
  {GLFW_KEY_LEFT_SHIFT, Nei::Key::LeftShift},
  {GLFW_KEY_LEFT_CONTROL, Nei::Key::LeftControl},
  {GLFW_KEY_LEFT_ALT, Nei::Key::LeftAlt},
  {GLFW_KEY_LEFT_SUPER, Nei::Key::LeftSuper},
  {GLFW_KEY_RIGHT_SHIFT, Nei::Key::RightShift},
  {GLFW_KEY_RIGHT_CONTROL, Nei::Key::RightControl,},
  {GLFW_KEY_RIGHT_ALT, Nei::Key::RightAlt},
  {GLFW_KEY_RIGHT_SUPER, Nei::Key::RightSuper},
  {GLFW_KEY_MENU, Nei::Key::Menu}
};

void framebufferSizeCallback(GLFWwindow*, int w, int h) {
  // same as WindowResize
}

void windowCloseCallback(GLFWwindow* window) {
  auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  win->close();

  WindowMessage msg(win, WindowMessage::MessageType::WindowClose);
  SEND(msg);
}

void windowPosCallback(GLFWwindow* window, int x, int y) {
  auto win = static_cast<Window *>(glfwGetWindowUserPointer(window));

  WindowMessage msg(win, WindowMessage::MessageType::WindowMove);
  msg.pos = {x, y};
  SEND(msg);
}

void windowSizeCallback(GLFWwindow* window, int w, int h) {
  auto win = static_cast<Window *>(glfwGetWindowUserPointer(window));
  WindowMessage msg(win, WindowMessage::MessageType::WindowResize);
  msg.size = {w, h};
  SEND(msg);
}

static ivec2 lastMousePos; // @TODO fix this
void cursorPosCallback(GLFWwindow* window, double x, double y) {
  auto win = static_cast<Window *>(glfwGetWindowUserPointer(window));

  if (win->isMouseCaptured())return;

  WindowMessage msg(win, WindowMessage::MessageType::MouseMove);
  msg.pos = {x, y};
  msg.delta = lastMousePos-msg.pos;
  lastMousePos = msg.pos;
  SEND(msg);
}

void scrollCallback(GLFWwindow* window, double x, double y) {
  auto win = static_cast<Window *>(glfwGetWindowUserPointer(window));

  if (win->isMouseCaptured())return;

  WindowMessage msg(win, WindowMessage::MessageType::MouseWheel);
  msg.delta = {x, y};
  SEND(msg);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto win = static_cast<Window *>(glfwGetWindowUserPointer(window));

  if (win->isMouseCaptured())return;

  WindowMessage msg(win, WindowMessage::MessageType::MouseButton);
  msg.button = button;
  msg.mods = mods;
  msg.press = action == GLFW_PRESS;
  SEND(msg);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto win = static_cast<Window *>(glfwGetWindowUserPointer(window));

  if (win->isKeyboardCaputred())return;

  WindowMessage msg(win, WindowMessage::MessageType::Key);
  msg.key = keyMap[key];
  msg.mods = mods;
  msg.press = action == GLFW_PRESS;
  SEND(msg);
}

//------------------------------------------------------------------------------

Window::Window(WindowCreateParams const& params) {
  this->params = params;
  createGlfwWindow();
}

Window::~Window() {
  destroyGlfwWindow();
}

void Window::createGlfwWindow() {
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // selected monitor and current mode
  auto selectedMonitor = getGlfwMonitor(params.display);
  auto mode = glfwGetVideoMode(selectedMonitor);
  ivec2 monitorSize{mode->width, mode->height};
  ivec2 monitorPosition;
  glfwGetMonitorPos(selectedMonitor, &monitorPosition.x, &monitorPosition.y);

  GLFWmonitor* usedMonitor = nullptr;
  ivec2 windowSize = params.size;

  // Create regular window
  glfwWindowHint(GLFW_DECORATED, !params.borderless);
  glfwWindowHint(GLFW_RESIZABLE, params.resizable);
  glfwWindowHint(GLFW_MAXIMIZED, params.maximized);

  // Fullscreen window flags
  if(params.fullscreen && params.fullscreenType == FULLSCREEN_WINDOW) {
    glfwWindowHint(GLFW_AUTO_ICONIFY, false);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    usedMonitor = selectedMonitor;
    windowSize = monitorSize;
  }

  // Borderless fullscreen flags
  if(params.fullscreen && params.fullscreenType == FULLSCREEN_BORDERLESS) {
    glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    windowSize = monitorSize;
  }


  glfwWindow = glfwCreateWindow(windowSize.x, windowSize.y,
    std::string(params.title).c_str(), usedMonitor, nullptr);

  // move borderless fullscreen window to selected monitor
  if(params.fullscreen && params.fullscreenType == FULLSCREEN_BORDERLESS) {
    setSize(monitorSize);
    setPosition(monitorPosition);
  }

  // Recenter window
  recenter();

  // Save ptr
  glfwSetWindowUserPointer(glfwWindow, this);

  // Callbacks
  glfwSetWindowCloseCallback(glfwWindow, windowCloseCallback);
  glfwSetWindowPosCallback(glfwWindow, windowPosCallback);
  glfwSetWindowSizeCallback(glfwWindow, windowSizeCallback);
  glfwSetFramebufferSizeCallback(glfwWindow, framebufferSizeCallback);
  glfwSetCursorPosCallback(glfwWindow, cursorPosCallback);
  glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
  glfwSetKeyCallback(glfwWindow, keyCallback);
  glfwSetScrollCallback(glfwWindow, scrollCallback);

  setResizable(params.resizable);
}


void Window::destroyGlfwWindow() {
  if(glfwWindow)
    glfwDestroyWindow(glfwWindow);
  glfwWindow = nullptr;
}

void Window::recenter() {
  auto monitor = getGlfwMonitor(params.display);
  auto mode = glfwGetVideoMode(monitor);
  ivec2 monitorPos;
  glfwGetMonitorPos(monitor, &monitorPos.x, &monitorPos.y);
  ivec2 monitorSize(mode->width, mode->height);

  auto windowSize = getSize();
  setPosition(monitorPos + monitorSize / 2 - windowSize / 2);
}

GLFWmonitor* Window::getGlfwMonitor(int index) {
  int count = 0;
  auto monitors = glfwGetMonitors(&count);
  index = clamp(index, 0, count - 1);
  return monitors[index];
}

void Window::setSize(ivec2 const& size) {
  if(!glfwWindow)return;
  glfwSetWindowSize(glfwWindow, size.x, size.y);
}

ivec2 Window::getSize() const {
  ivec2 res;
  if(!glfwWindow)return res;
  glfwGetWindowSize(glfwWindow, &res.x, &res.y);
  return res;
}

void Window::setPosition(ivec2 const& pos) {
  if(!glfwWindow)return;
  glfwSetWindowPos(glfwWindow, pos.x, pos.y);
}

ivec2 Window::getPosition() const {
  ivec2 res;
  if(!glfwWindow)return res;
  glfwGetWindowPos(glfwWindow, &res.x, &res.y);
  return res;
}

void* Window::getNativeHandle() const {
#ifdef WIN32
  return glfwGetWin32Window(glfwWindow);
#endif
  return nullptr;
}

void Window::close() {
  destroyGlfwWindow();
}

bool Window::isClosed() const {
  return glfwWindow == nullptr;
}

void Window::setTitle(std::string const& title) {
  glfwSetWindowTitle(glfwWindow, title.c_str());
}

bool Window::isMouseCaptured() {
  auto gui = Application::getInstance()->getGui();
  gui->setCurrent(this);
  return ImGui::GetIO().WantCaptureMouse;
}

bool Window::isKeyboardCaputred() {
  auto gui = Application::getInstance()->getGui();
  gui->setCurrent(this);
  return ImGui::GetIO().WantCaptureKeyboard;
}

void Window::setResizable(bool resizable)
{
  if (!glfwWindow) return;
  glfwSetWindowAttrib(glfwWindow, GLFW_RESIZABLE, resizable);
}

void Window::setFullscreen(bool fullscreen) {
  if(params.fullscreen == fullscreen) return;
  params.fullscreen = fullscreen;

  switch(params.fullscreenType) {
    case FULLSCREEN_BORDERLESS: {
      // Borderless - recreate
      destroyGlfwWindow();
      createGlfwWindow();
      // send resize event
      auto size = getSize();
      windowSizeCallback(glfwWindow, size.x, size.y);
      break;
    }
    case FULLSCREEN_WINDOW: {
      // Fullscreen window - assign monitor
      if(fullscreen) {
        auto selectedMonitor = getGlfwMonitor(params.display);
        auto mode = glfwGetVideoMode(selectedMonitor);
        glfwSetWindowMonitor(glfwWindow, selectedMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      } else {
        glfwSetWindowMonitor(glfwWindow, nullptr, 0, 0, params.size.x, params.size.y, 0);
        recenter();
      }
      break;
    }
  }
}

bool Window::getFullscreen() const {
  return params.fullscreen;
}
