#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <string>

#include "../common/assets.h"

namespace Renderer
{
	class Texture;
	class TextureManager
	{
	public:
		TextureManager() = default;
		virtual int AddTexture(const std::string& name, const Asset::Image& image) = 0;
		virtual int DeleteTexture(const std::string& name) = 0;
		virtual std::shared_ptr<Texture> GetTexture(const std::string& name)
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
		virtual int Cleanup()
		{
			textures_map.clear();
			return 0;
		}
	protected:
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures_map;
	};

	
}

#endif //!TEXTURE_MANAGER_H