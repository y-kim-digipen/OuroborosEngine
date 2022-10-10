#pragma once
#include "gui_definitions.h"
#include <string>
#include <array>
#include "base.h"
#include "../../common/assets.h"

namespace OE
{
	namespace GUI
	{
		class MaterialConfigure : public GUI_Base
		{
		public:
			MaterialConfigure(const std::string& title, const std::string& material_name = std::string());
			virtual void Draw() override;
		private:
			void draw_internal();
			bool is_exist_material;

			std::string reference_material;
			
			DefaultSizeBuffer material_name_buf{""};
			Shared::PBR_Structure pbr_material_info;

			bool using_texture = false;
			float metallic = 0.0, roughness = 0.0, ao = 0.0;
			glm::vec3 albedo{ 0.0 };

			std::array<std::string, Shared::COUNT> texture_names;
			bool use_metallic_roughness = false;
			bool invert_roughness = false;

			ImGuiTextFilter filter;
		};
	}
}

