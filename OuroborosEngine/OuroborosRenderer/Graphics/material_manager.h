#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include <memory>
#include <unordered_map>
#include <string>

namespace Renderer {

	struct Material;

	class MaterialMananger {
	public:
		MaterialMananger() = default;
		virtual ~MaterialMananger() = default;
		virtual int AddMaterial() = 0;

	protected:
		std::unordered_map<std::string, std::unique_ptr<Material>> material_map;
	};

}


#endif // !MATERIAL_MANAGER_H
