#pragma once
#include "base.h"
#include "../core/engine_definitions.h"
#include <ImGuizmo.h>
namespace OE
{
	namespace GUI
	{
		class GizmoEditor : public GUI_Base
		{
		public:
			struct GizmoEditingConfig
			{
				bool perspective = true;
				ImGuizmo::MODE mode = ImGuizmo::MODE::LOCAL;
				ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
				float snap_amount = 0.5f;
				bool do_snap = false;
			};
			GizmoEditor() : GUI_Base("Gizmo Editor") {}
			void Draw() override;
			static bool SetSelectedEntity(ecs_ID entityID);
			static ecs_ID GetSelectedEntity() { return selected_entity; }
			static GizmoEditingConfig* GetConfig() { return &config; }
		private:
			inline static GizmoEditingConfig config;
			inline static ecs_ID selected_entity = -1;
		};
	}
}
