#include "viewport.h"
#include "../engine.h"
void OE::GUI::ViewPort::Draw()
{
	Engine::RegisterEvent(Engine::EventFunctionType::END_OF_RENDERER_END_FRAME, std::bind(&ViewPort::draw_internal, this));
}

void OE::GUI::ViewPort::draw_internal()
{
    const bool fit_to_frame = OE::Engine::gui_manager.GetBehavior({ "System", "Viewport" }, "Fit to frame")->open;

    ImGui::Begin(name.c_str(), &open);

    auto context = Engine::window->GetWindowData().RenderContextData;
    auto& texture_manager = context->texture_manager;
    const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();

    dynamic_cast<Renderer::VulkanContext*>(context.get())->UpdateViewportDescriptorSet(*TextureID, 0);
    ImVec2 viewport_size = ImGui::GetWindowSize();
    if(fit_to_frame)
    {
        glm::vec2 render_size = { Engine::window->GetWidth(), Engine::window->GetHeight() };
        float aspect_ratio = render_size.y / render_size.x;

        glm::ivec2 target_size = glm::vec2{ viewport_size.x, viewport_size.x * aspect_ratio };

        ImGui::Image(*TextureID, ImVec2{static_cast<float>(target_size.x), static_cast<float>(target_size.y)});
    }
    else
    {
        ImGui::Image(*TextureID, viewport_size);
    }

    ImGui::End();
}
