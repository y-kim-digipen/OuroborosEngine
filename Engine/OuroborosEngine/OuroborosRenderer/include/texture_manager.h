#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <string>

#include <common/assets.h>

namespace Renderer
{
	class VulkanTexture;

	class TextureManager
	{

	public:
		TextureManager() = default;
		virtual int AddTexture(const std::string& name, const Asset::Image& image) = 0;
		virtual int DeleteTexture(const std::string& name) = 0;
		virtual std::shared_ptr<VulkanTexture> GetTexture(const std::string& name)
		{
			if(auto iter = textures_map.find(name); iter != textures_map.end())
			{
				return iter->second;
			}
			else
			{
				return nullptr;
			}
		}
		virtual std::string GetName(std::shared_ptr<VulkanTexture> texture)
		{
			if(name_map.contains(texture))
			{
				return name_map[texture];
			}
			return "";
		}
		virtual int Cleanup()
		{
			textures_map.clear();
			return 0;
		}

		const auto& GetRawData()
		{
			return textures_map;
		}
	protected:
		std::unordered_map<std::string, std::shared_ptr<VulkanTexture>> textures_map;
		std::unordered_map<std::shared_ptr<VulkanTexture>, std::string> name_map;
	};

	
}

#endif //!TEXTURE_MANAGER_H