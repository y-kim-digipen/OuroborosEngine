#include "viewport.h"
#include "../core/engine.h"
#include <ImGuizmo.h>
#include <imgui/imgui_internal.h>

#include "gizmo_editor.h"
void OE::GUI::ViewPort::Draw()
{
    auto fn = std::bind(&ViewPort::draw_internal, this);
	Engine::RegisterEvent(Engine::EventFunctionType::END_OF_RENDERER_END_FRAME, fn);
}

void OE::GUI::ViewPort::draw_internal()
{
    const bool fit_to_frame = OE::Engine::gui_manager.GetBehavior({ "System", "Viewport" }, "Fit to frame")->open;


    ImGuiWindowFlags flag = ImGuiActivateFlags_None;
    flag |= ImGuiWindowFlags_NoScrollbar;
    flag |= ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin(name.c_str(), &open, flag);

    auto context = Engine::window->GetWindowData().RenderContextData;
    auto& texture_manager = context->texture_manager;
    const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();

    dynamic_cast<Renderer::VulkanContext*>(context.get())->UpdateViewportDescriptorSet(*TextureID, 0);
    ImVec2 viewport_size = ImGui::GetWindowSize();
    item_pos_x = ImGui::GetWindowPos().x;
    item_pos_y = ImGui::GetWindowPos().y;
    if(fit_to_frame)
    {
        glm::ivec2 target_size = glm::vec2{ viewport_size.x, viewport_size.x / target_aspect_ratio };
        item_width_x = target_size.x;
        item_height_y = target_size.y;

        ImGui::Image(*TextureID, ImVec2{static_cast<float>(target_size.x), static_cast<float>(target_size.y)});
    }
    else
    {
        ImGui::Image(*TextureID, viewport_size);
        item_width_x = viewport_size.x;
        item_height_y = viewport_size.y;

        if(abs(static_cast<float>(item_height_y) / item_width_x - target_aspect_ratio) > std::numeric_limits<float>::epsilon())
        {
            Engine::ChangeWindowSize(item_width_x, item_height_y);
        }
    }


    draw_gizmo();

    ImGui::End();
}

void OE::GUI::ViewPort::draw_gizmo()
{
	const GizmoEditor* gizmo_editing_config_panel = Engine::gui_manager.TryGetPanel<OE::GUI::GizmoEditor>();
    const GizmoEditor::GizmoEditingConfig* config = GizmoEditor::GetConfig();
    if(gizmo_editing_config_panel == nullptr)
    {
        return;
    }
    ecs_ID selected_entity = gizmo_editing_config_panel->GetSelectedEntity();
    if(selected_entity < 0)
    {
        return;
    }

    auto& transform_component = Engine::GetECSManager().GetComponent<TransformComponent>(selected_entity);
    auto active_camera = Engine::Get().GetActiveCamera();
    if(active_camera == nullptr)
    {
        return;
    }
    glm::mat4 mat = transform_component.GetMatrix();

    ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
    static ImGuiWindowFlags gizmoWindowFlags = 0;


    ImGuizmo::Enable(true);
    float windowWidth = (float)ImGui::GetWindowWidth();
    float windowHeight = (float)ImGui::GetWindowHeight();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
    viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
    viewManipulateTop = ImGui::GetWindowPos().y;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    auto context = ImGui::GetCurrentContext();
    gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();

    glm::mat4 perspective_mat = active_camera->GetPerspectiveMatrix();
    perspective_mat[1][1] *= -1;
    glm::mat4 identity{ 1.f };
    ImGuizmo::DrawGrid(&active_camera->GetViewMatrix()[0][0], &perspective_mat[0][0], &identity[0] [0], 10.f);
    ImGuizmo::Manipulate(&active_camera->GetViewMatrix()[0][0], &perspective_mat[0][0], 
        config->operation, config->mode, &mat[0][0], nullptr, config->do_snap ? &config->snap_amount : nullptr);

    glm::vec3 translate;
    glm::vec3 rotation;
    glm::vec3 scale;

    mat = glm::inverse(transform_component.GetParentMatrix()) * mat;
    ImGuizmo::DecomposeMatrixToComponents(&mat[0][0], &translate.x, &rotation.x, &scale.x);


    transform_component.SetPosition(translate);
    transform_component.SetRotation(rotation);
    transform_component.SetScale(scale);

}

std::pair<uint16_t, uint16_t> OE::GUI::ViewPort::GetViewPortPos()
{
    return std::make_pair(item_pos_x, item_pos_y);
}

std::pair<uint16_t, uint16_t> OE::GUI::ViewPort::GetViewPortSize()
{
    return std::make_pair(item_width_x, item_height_y);
}

void OE::GUI::ViewPort::SetTargetRenderSize(uint16_t width, uint16_t height)
{
    target_render_width = width;
    target_render_height = height; 
    target_aspect_ratio = static_cast<float>(target_render_width) / target_render_height;
}
