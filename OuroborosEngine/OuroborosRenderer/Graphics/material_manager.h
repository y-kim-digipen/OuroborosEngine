#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include "material.h"

#include <memory>
#include <unordered_map>
#include <string>

namespace Renderer {

	class MaterialMananger {
	public:
		MaterialMananger() = default;
		virtual ~MaterialMananger() = default;
		virtual int AddMaterial(std::string material_name, const Asset::MaterialData& data) = 0;
		virtual Material* GetMaterial(const std::string& material_name) = 0;
		virtual int ChangeMaterial(std::string material_name, const Asset::MaterialData& data) = 0;
		void ClearMaterialDatum()
		{
			material_map.clear();
		}
		const auto& GetMaterialDatum() const
		{
			return material_map;
		}
		virtual void Cleanup();
	protected:
		std::unordered_map<std::string, std::unique_ptr<Material>> material_map;
	};

}


#endif // !MATERIAL_MANAGER_H
