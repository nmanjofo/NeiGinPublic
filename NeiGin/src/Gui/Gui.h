#pragma once

#include "NeiGinBase.h"
#include "NeiVu/DeviceContext.h"

#include "imgui/imgui.h"

namespace Nei {
  class NEIGIN_EXPORT Gui : public Object {
  public:
    Gui();
    virtual ~Gui();

    static Gui* getInstance();

    // init for general use
    void init(DeviceContext* dc);

    // init for use in separate render pass
    void init(Vu::Texture2D* attachment, vk::ImageLayout initialLayout = vk::ImageLayout::eColorAttachmentOptimal,
              vk::ImageLayout finalLayout = vk::ImageLayout::eTransferSrcOptimal);
    // init for use in existing renderpass
    void init(Vu::RenderPass* renderPass);

    void createFontAtlas();
    bool isInitialized();

    void setCurrent(Window* window);

    void begin(Window* window, Vu::CommandBuffer* cmd);
    void end();

    void newFrame();
    void render(CommandBuffer* cmd);

    static void textColor(float r, float g, float b, float a = 1);
    static void label(std::string const& text, int x = 0, int y = 0, int width = 200, int height = 100);
    static void fpsLabel(vec3 color = vec3(1, 1, 0), int x = 0, int y = 0, int width = 200, int height = 20);

  protected:
    void createRenderPass(Vu::Texture2D* attachment, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);
    void createFrameBuffer();

    static Gui* instance;

    std::map<Window*, ImGuiContext*> contextMap;
    Vu::CommandBuffer* activeCommandBuffer = nullptr;
    ImGuiContext* imguiContext = nullptr;
    ImFontAtlas* sharedAtlas = nullptr;

    Ptr<Vu::DescriptorPool> descriptorPool;

    // for separate render pass
    uvec2 size;
    Ptr<DeviceContext> deviceContext;
    Ptr<Texture2D> attachment;
    Ptr<RenderPass> renderPass;
    Ptr<Framebuffer> framebuffer;

  };
};
