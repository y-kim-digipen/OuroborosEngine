#include "viewport.h"
#include "../engine.h"
void OE::GUI::ViewPort::Draw()
{
    auto fn = std::bind(&ViewPort::draw_internal, this);
	Engine::RegisterEvent(Engine::EventFunctionType::END_OF_RENDERER_END_FRAME, fn);
}

void OE::GUI::ViewPort::draw_internal()
{
    ImGui::Begin("viewport");
    auto context = Engine::window->GetWindowData().RenderContextData;
    auto& texture_manager = context->texture_manager;
    const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();

    dynamic_cast<Renderer::VulkanContext*>(context.get())->UpdateViewportDescriptorSet(*TextureID, 0);
    ImGui::Image(*TextureID, ImVec2(800, 600));
    ImGui::End();
}
