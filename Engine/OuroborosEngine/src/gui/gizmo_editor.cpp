#include "gizmo_editor.h"
#include "../core/engine.h"
#include <ImGuizmo.h>

#include "viewport.h"

void OE::GUI::GizmoEditor::Draw()
{
	//if (selected_entity < 0 || Engine::ecs_manager.GetEntity(selected_entity).alive == false)
	//{
	//	selected_entity = -1;
	//	return;
	//}

    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Gizmo editor", &open, window_flags))
    {
        ImGui::Text("Selected Entity %s", (selected_entity < 0 ? "None" : std::to_string(selected_entity).c_str()));
        ImGui::Separator();
        if(ImGui::RadioButton("Translate", config.operation == ImGuizmo::TRANSLATE))
			config.operation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", config.operation == ImGuizmo::ROTATE))
			config.operation = ImGuizmo::ROTATE;
        ImGui::SameLine();
    	if (ImGui::RadioButton("Scale", config.operation == ImGuizmo::SCALE))
			config.operation = ImGuizmo::SCALE;
        ImGui::Separator();
        if (ImGui::RadioButton("World", config.mode == ImGuizmo::WORLD))
			config.mode = ImGuizmo::WORLD;
        ImGui::SameLine();
        if (ImGui::RadioButton("Local", config.mode == ImGuizmo::LOCAL))
			config.mode = ImGuizmo::LOCAL;
        ImGui::Separator();
        if (ImGui::RadioButton("Snap", config.do_snap))
			config.do_snap = true;
        ImGui::SameLine();
        ImGui::InputFloat("##Snap value", &config.snap_amount);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (open && ImGui::MenuItem("Close")) open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

bool OE::GUI::GizmoEditor::SetSelectedEntity(ecs_ID entityID)
{
	if(Engine::ecs_manager.GetEntity(entityID).alive)
	{
		selected_entity = entityID;
        return true;
	}
    return false;
}
