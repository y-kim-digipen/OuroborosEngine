#include "material_manager.h"

int Renderer::MaterialMananger::ChangeMaterialName(std::string original_name, std::string name_to_change)
{
	if(material_map.contains(original_name) == false)
	{
		return 1;
	}
	auto nodeHandler = material_map.extract(original_name);
	nodeHandler.key() = name_to_change;
	material_map.insert(std::move(nodeHandler));
	return 0;
}

void Renderer::MaterialMananger::Cleanup()
{
	material_map.clear();
}
