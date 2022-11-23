#include "material_asset_manager.h"
#include <common/assets.h>
#include "../core/engine.h"
#include "../serializing/shared_serialization_impl.h"

int OE::MaterialAssetManager::LoadAsset(const std::string& name, MaterialAssetManager::AssetType asset)
{
	{
		Asset::MaterialData data;
		switch (asset.pbr_mat.GetMode())
		{
		case Shared::MaterialMode::TEXTURE:
		{
			const Shared::PBR_TEXTURE& texture_material = *asset.pbr_mat.structure.texture;
			data.has_albedo_texture = !texture_material.texture_names[Shared::ALBEDO].empty();
			data.has_normal_texture = !texture_material.texture_names[Shared::NORMAL].empty();
			if (texture_material.use_combined_metallic_roughness)
			{
				data.has_metalroughness_texture = !texture_material.texture_names[Shared::METALLIC_ROUGHNESS].empty();
			}
			else
			{
				data.has_metalic_texture = !texture_material.texture_names[Shared::METALLIC].empty();
				data.has_roughness_texture = !texture_material.texture_names[Shared::ROUGHNESS].empty();
				data.is_smoothness = texture_material.is_smoothness;
			}
			data.has_ao_texture = !texture_material.texture_names[Shared::AO].empty();
			data.has_emissive_texture = !texture_material.texture_names[Shared::EMISSIVE].empty();
			break;
		}
		case Shared::MaterialMode::NONE_TEXTURE:
		{
			const Shared::PBR_NONE_TEXTURE& non_texture_material = *asset.pbr_mat.structure.non_texture;
			data.albedo = non_texture_material.albedo;
			data.metallic = non_texture_material.metallic;
			data.roughness = non_texture_material.roughness;
			data.ao = non_texture_material.ao;
			break;
		}
		}
		Engine::window->GetWindowData().RenderContextData->material_manager->AddMaterial(name, data);
		const auto& material = Engine::window->GetWindowData().RenderContextData->material_manager->GetMaterial(name);
		if(asset.pbr_mat.GetMode() == Shared::MaterialMode::TEXTURE)
		{
			const Shared::PBR_TEXTURE& texture_material = *asset.pbr_mat.structure.texture;
			auto GetTexture = [](const std::string& texture_name)
			{
				return Engine::window->GetWindowData().RenderContextData->texture_manager->GetTexture(texture_name);
			}; 

			for(int i = 0; i < Shared::COUNT; ++i)
			{
				auto type = static_cast<Shared::PBR_TEXTURE_TYPES>(i);
				if(material->DoUseTexture(type))
				{
					material->SetTexture(type, GetTexture(texture_material.texture_names[i]));
				}
			}
		}
	}
	asset.name = name;
	return AssetManager<Asset::Material>::LoadAsset(name, std::move(asset));
}

int OE::MaterialAssetManager::LoadAsset(const std::string& name)
{
	return 0;
}

int OE::MaterialAssetManager::UnloadAsset(const std::string& name)
{
	return AssetManager<Asset::Material>::UnloadAsset(name);
}

void OE::MaterialAssetManager::Deserialize(std::string asset_name, YAML::Node node)
{
	Asset::Material asset{};
	asset.name = asset_name;
	node >> asset;
	LoadAsset(asset_name, asset);
}
