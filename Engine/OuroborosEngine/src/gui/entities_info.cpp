#include "entities_info.h"
#include "../core/engine.h"
#include "components_draw.h"
#include <ImGuizmo.h>

static std::vector<std::string> popup_menu_strings =
{ "Add Component", "Add Entity", "Delete Entity", "Delete Component", "System", "Remove parent"};

enum PopupMenuTypes
{
	COMPONENT_ADD, ENTITY_ADD, ENTITY_DELETE, COMPONENT_DELETE, SYSTEM, REMOVE_PARENT
};

void OE::GUI::EntitesInfo::Draw()
{
	ImGui::Begin(name.c_str(), &open);
	ImGui::BeginChild((name + " child").c_str());
	if(is_popup_menu_opened == false)
	{
		selected_entity = -1;
	}
	auto& engine = Engine::Get();
	using component_list = decltype(std::declval<ECS_Manager>().component_manager)::ComponentList;
	if(ImGui::BeginTable("Entities", 1, ImGuiTableFlags_Reorderable))
	{
		Engine::ecs_manager.ForEntities([this](ecs_ID entityID)
		{
			auto& entity = Engine::ecs_manager.GetEntity(entityID);
			if (entity.hierarchy.HasParent() == false) { DrawSingleEntity(entityID); }
		});
		ImGui::EndTable();
	}
	is_popup_menu_opened = ImGui::BeginPopupContextWindow();
	if (is_popup_menu_opened)
	{
		if (selected_entity >= 0)
		{
			if (ImGui::MenuItem(popup_menu_strings[ENTITY_DELETE].c_str()))
			{
				Engine::ecs_manager.DeleteEntity(selected_entity);
			}
			if (ImGui::BeginMenu(popup_menu_strings[COMPONENT_ADD].c_str()))
			{
				using TComponents = decltype(Engine::ecs_manager.component_manager)::ComponentList;

				brigand::for_each<TComponents>([this](auto type)
					{
						using TComponent = typename decltype(type)::type;
						const bool has_this_component = OE::Engine::ecs_manager.HasComponent<TComponent>(selected_entity);
						if (has_this_component)
						{
							ImGui::BeginDisabled();
						}
						if (ImGui::MenuItem(typeid(TComponent).name(), nullptr, has_this_component))
						{
							OE::Engine::ecs_manager.AddComponent<TComponent>(selected_entity);
						}
						if (has_this_component)
						{
							ImGui::EndDisabled();
						}
					});
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(popup_menu_strings[COMPONENT_DELETE].c_str()))
			{
				using TComponents = decltype(Engine::ecs_manager.component_manager)::ComponentList;

				brigand::for_each<TComponents>([this](auto type)
					{
						using TComponent = typename decltype(type)::type;
						const bool has_this_component = OE::Engine::ecs_manager.HasComponent<TComponent>(selected_entity);
						if (!has_this_component)
						{
							ImGui::BeginDisabled();
						}
						if (ImGui::MenuItem(typeid(TComponent).name(), nullptr, has_this_component))
						{
							OE::Engine::ecs_manager.DeleteComponent<TComponent>(selected_entity);
						}
						if (!has_this_component)
						{
							ImGui::EndDisabled();
						}
					});

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(popup_menu_strings[SYSTEM].c_str()))
			{
				using TSystems = decltype(Engine::ecs_manager)::SystemStorage::system_list;

				brigand::for_each<TSystems>([this](auto type)
					{
						using TSystem = typename decltype(type)::type::type;
						const bool has_this_system = OE::Engine::ecs_manager.HasSystem<TSystem>(selected_entity);
						if (ImGui::MenuItem(typeid(TSystem).name(), nullptr, has_this_system))
						{
							if (!has_this_system)
							{
								OE::Engine::ecs_manager.AddSystem<TSystem>(selected_entity);
							}
							else
							{
								OE::Engine::ecs_manager.DeleteSystem<TSystem>(selected_entity);
							}
						}
					});
				ImGui::EndMenu();
			}
			if(Engine::ecs_manager.GetEntity(selected_entity).hierarchy.HasParent())
			{
				if (ImGui::MenuItem(popup_menu_strings[REMOVE_PARENT].c_str()))
				{
					Engine::ecs_manager.GetEntity(selected_entity).hierarchy.SetParent(-1);
				}
			}
		}
		else
		{
			if (ImGui::MenuItem(popup_menu_strings[ENTITY_ADD].c_str()))
			{
				Engine::ecs_manager.CreateEntity();
			}
			
		}
		ImGui::EndPopup();
	}

	
	ImGui::EndChild();
	ImGui::End();

	if (selected_entity >= 0)
	{
		//auto entity_to_draw_gizmo = Engine::GetECSManager().GetEntity(selected_entity);
		if (Engine::GetECSManager().HasComponent<TransformComponent>(selected_entity))
		{
			auto& transform_component = Engine::GetECSManager().GetComponent<TransformComponent>(selected_entity);
			glm::mat4 mat = transform_component.GetMatrix();
			//ImGuizmo::SetOrthographic(false);
			//ImGuizmo::SetDrawlist();
			//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			auto active_camera = Engine::Get().GetActiveCamera();

			//ImGuizmo::Manipulate(&active_camera.GetViewMatrix()[0][0], &active_camera.GetPerspectiveMatrix()[0][0],
			//	ImGuizmo::TRANSLATE, ImGuizmo::MODE::WORLD, &transform_component.GetMatrix()[0][0]);

			static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
			static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
			if (ImGui::IsKeyPressed(90))
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed(69))
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed(82)) // r Key
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(&mat[0][0], matrixTranslation, matrixRotation, matrixScale);
			ImGui::InputFloat3("Tr", matrixTranslation);
			ImGui::InputFloat3("Rt", matrixRotation);
			ImGui::InputFloat3("Sc", matrixScale);
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &mat[0][0]);

			if (mCurrentGizmoOperation != ImGuizmo::SCALE)
			{
				if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
					mCurrentGizmoMode = ImGuizmo::LOCAL;
				ImGui::SameLine();
				if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
					mCurrentGizmoMode = ImGuizmo::WORLD;
			}
			//static bool useSnap(false);
			//if (ImGui::IsKeyPressed(83))
			//	useSnap = !useSnap;
			//ImGui::Checkbox("", &useSnap);
			//ImGui::SameLine();
			//vec_t snap;
			//switch (mCurrentGizmoOperation)
			//{
			//case ImGuizmo::TRANSLATE:
			//	snap = config.mSnapTranslation;
			//	ImGui::InputFloat3("Snap", &snap.x);
			//	break;
			//case ImGuizmo::ROTATE:
			//	snap = config.mSnapRotation;
			//	ImGui::InputFloat("Angle Snap", &snap.x);
			//	break;
			//case ImGuizmo::SCALE:
			//	snap = config.mSnapScale;
			//	ImGui::InputFloat("Scale Snap", &snap.x);
			//	break;
			//}
			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			ImGuizmo::Manipulate(&active_camera.GetViewMatrix()[0][0], &active_camera.GetPerspectiveMatrix()[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &mat[0][0], NULL, NULL);


			//ImGuizmo::
		}
	}
}

void OE::GUI::EntitesInfo::DrawSingleEntity(ecs_ID entityID)
{
	auto& entity = Engine::ecs_manager.GetEntity(entityID);
	ImGui::PushID(entityID);
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		
		if (entity.alive)
		{
			std::string strID = std::to_string(entityID);
			if (Engine::ecs_manager.HasComponent<TagComponent>(entity.myID))
			{
				TagComponent& tc = Engine::ecs_manager.GetComponent<TagComponent>(entity.myID);
				strID.append("_" + tc.tag);
			}
			if (ImGui::TreeNode(strID.c_str()))
			{
				{//Drag and drop
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("Set parent", &entityID, sizeof(ecs_ID));
						ImGui::Text("Set parent");
						ImGui::EndDragDropSource();
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Set parent"))
						{
							const ecs_ID payload_data = *static_cast<const ecs_ID*>(payload->Data);
							Engine::ecs_manager.GetEntity(payload_data).hierarchy.SetParent(entityID);
						}
						ImGui::EndDragDropTarget();
					}
				}
				if (ImGui::IsItemHovered())
				{
					selected_entity = entityID;
				}

				for (const ecs_ID child_ID : entity.hierarchy.GetChild())
				{
					DrawSingleEntity(child_ID);
				}

				using TComponents = decltype(Engine::ecs_manager.component_manager)::ComponentList;

				brigand::for_each<TComponents>([entityID](auto type)
					{
						using TComponent = typename decltype(type)::type;
						const bool has_this_component = OE::Engine::ecs_manager.HasComponent<TComponent>(entityID);
						if (has_this_component)
						{
							ComponentDrawFunction<TComponent>(entityID);
						}
					});

				ImGui::TreePop();
			}
			else
			{
				if (ImGui::IsItemHovered())
				{
					selected_entity = entityID;
				}
			}
		}
	}
	ImGui::PopID();
}
