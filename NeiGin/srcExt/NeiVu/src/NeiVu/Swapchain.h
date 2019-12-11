#pragma once

#include "NeiVuBase.h"

namespace Nei::Vu {
  struct SwapchainCreateInfo {
    DeviceContext* deviceContext = nullptr;
    void* windowHandle = nullptr;
    bool vsync = false;
    uint32 imageCount = 2;
  };

  class NEIVU_EXPORT Swapchain : public Object {
  public:
    Swapchain(SwapchainCreateInfo const& info);
    Swapchain(void* handle, DeviceContext* deviceContext);
    ~Swapchain();

    void resize();
    vk::Image getImage(int i) { return swapchainImages[i]; }

    vk::Semaphore getPresentSemaphore() const { return presentSemaphores[currentImage]; }
    vk::Semaphore getAcquireSemaphore() const { return acquireSemaphore[aqIndex]; }

    void next();
    void copy(CommandBuffer* cmd, Texture* texture);
    void present();

    void begin() { next(); }
    void end() { present(); }

    bool isValid() const;

    operator vk::SwapchainKHR() const { return swapchain; }
  protected:
    void init(SwapchainCreateInfo const& info);
    void createSurface();
    void createSwapchain();

    void destroySwapchain();
    void destroySurface();

    uint32 imageCount = 0;
    bool vsync = false;
    void* windowHandle = nullptr;
    Ptr<DeviceContext> deviceContext;
    vk::SurfaceFormatKHR surfaceFormat;
    vk::SurfaceKHR surface;
    vk::SwapchainKHR swapchain;

    std::vector<vk::Image> swapchainImages;
    std::vector<vk::ImageView> swapchainImageViews;

    std::vector<vk::Semaphore> acquireSemaphore;
    std::vector<vk::Semaphore> presentSemaphores;

    ivec2 size;

    uint32 currentImage = 0;

    uint aqIndex = 0;
  };
};
