#include "Gui.h"

#include "NeiVu/Context.h"
#include "NeiVu/CommandBuffer.h"
#include "Application/Window.h"
#include "NeiVu/Texture.h"
#include "NeiVu/RenderPass.h"
#include "NeiVu/Framebuffer.h"
#include "NeiVu/DescriptorSetLayout.h"
#include "NeiVu/DescriptorPool.h"

#include "Gui/imgui/imgui.h"
#include "Gui/imgui/imgui_impl_glfw.h"
#include "Gui/imgui/imgui_impl_vulkan.h"

#include <sstream>

using namespace Nei;

Gui* Gui::instance = nullptr;

Gui::Gui() {
  nei_assert(!instance);
  
  instance = this;

  sharedAtlas = new ImFontAtlas;
}

Gui::~Gui() {
  if (!imguiContext) return;
  delete sharedAtlas;

  for (auto& [win,ctx] : contextMap) {
    ImGui::SetCurrentContext(ctx);
    ImGui_ImplGlfw_Shutdown();
  }
  ImGui::DestroyContext(imguiContext);

  instance = nullptr;
}

Gui* Gui::getInstance() {
  if (!instance)instance = new Gui();
  return instance;
}

void Gui::init(DeviceContext* dc) {
  renderPass = new RenderPass(dc);
  renderPass->addAttachment(vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal);
  renderPass->create();
  init(renderPass);
}

void Gui::init(Vu::Texture2D* attachment, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout) {
  deviceContext = attachment->getDeviceContext();
  this->attachment = attachment;
  size = attachment->getSize();

  renderPass = new Vu::RenderPass(deviceContext);
  renderPass->addAttachment(attachment->getFormat(), initialLayout, finalLayout, vk::AttachmentLoadOp::eLoad);
  renderPass->create();

  createFrameBuffer();

  init(renderPass);
}

void Gui::init(Vu::RenderPass* renderPass) {
  deviceContext = renderPass->getDeviceContext();
  this->renderPass = renderPass;

  imguiContext = ImGui::CreateContext(sharedAtlas);
  ImGui::SetCurrentContext(imguiContext);

  Ptr layout = new DescriptorSetLayout(deviceContext);
  layout->addDescriptor(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eAll);

  descriptorPool = new Vu::DescriptorPool(deviceContext, layout);
  descriptorPool->create(1024);

  ImGui_ImplVulkan_InitInfo ii{};
  ii.Instance = deviceContext->getContext()->getVkInstance();
  ii.PhysicalDevice = deviceContext->getVkPhysicalDevice();
  ii.Device = deviceContext->getVkDevice();
  ii.Queue = deviceContext->getMainQueue();
  ii.QueueFamily = deviceContext->getMainQueueIndex();
  ii.DescriptorPool = **descriptorPool;
  ii.PipelineCache = deviceContext->getPipelineCache();

  ImGui_ImplVulkan_Init(&ii, **renderPass);

  if (!sharedAtlas->IsBuilt())
    createFontAtlas();

}

void Gui::createFontAtlas() {
  auto cmd = deviceContext->getSingleUseCommandBuffer();
  cmd->begin();
  ImGui_ImplVulkan_CreateFontsTexture(**cmd);
  cmd->end();
  cmd->submit();

  ImGui_ImplVulkan_InvalidateFontUploadObjects();
}

bool Gui::isInitialized() {
  return !!deviceContext;
}

void Gui::setCurrent(Window* window) {
  auto ctx = contextMap[window];
  if (!ctx) {
    ctx = ImGui::CreateContext(sharedAtlas);
    contextMap[window] = ctx;
    ImGui::SetCurrentContext(ctx);
    ImGui_ImplGlfw_InitForVulkan(window->getGlfwWindow(), true);
  } else {
    ImGui::SetCurrentContext(ctx);
  }
}

void Gui::begin(Window* window, Vu::CommandBuffer* cmd) {
  if (attachment) {
    if (attachment->getSize() != size) {
      deviceContext->getMainQueue().waitIdle();
      createFrameBuffer();
    }

    renderPass->begin(cmd, framebuffer);
    cmd->viewport(size);
    cmd->scissor(size);
  }

  setCurrent(window);
  newFrame();

  activeCommandBuffer = cmd;
}

void Gui::end() {
  ProfileGPU(activeCommandBuffer, "GUI");

  render(activeCommandBuffer);

  if (attachment) {
    renderPass->end();
  }
}

void Gui::newFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Gui::render(CommandBuffer* cmd) {
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), **cmd);
}


void Gui::textColor(float r, float g, float b, float a) {
  ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(r, g, b, a);
}

void Gui::label(std::string const& text, int x, int y, int width, int height) {
  static int id = 0;
  IMGUI_ONCE_UPON_A_FRAME {
    id = 0;
  }
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowBorderSize = 0.0f;
  ImGui::Begin(std::to_string(id++).c_str(), nullptr, ImVec2(float(width), float(height)), 0.0f,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
               ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
  ImGui::SetWindowPos(ImVec2(float(x), float(y)));
  ImGui::TextUnformatted(text.c_str());
  ImGui::End();
}

void Gui::fpsLabel(vec3 color, int x, int y, int width, int height) {
  textColor(color.x,color.y,color.z,1);
  auto fps = ImGui::GetIO().Framerate;
  std::ostringstream s;
  s.precision(2);
  s << "fps:" << std::fixed << fps;
  label(s.str(), x, y, width, height);
  textColor(1,1,1,1);
}

void Gui::createFrameBuffer() {
  framebuffer = new Vu::Framebuffer(deviceContext);
  framebuffer->addTexture(attachment);
  framebuffer->setSize(attachment->getSize());
  framebuffer->create(renderPass);
  size = attachment->getSize();
}
