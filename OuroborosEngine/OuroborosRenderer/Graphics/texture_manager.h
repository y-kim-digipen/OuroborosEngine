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

	protected:
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures_map;
	};
}

#endif //!TEXTURE_MANAGER_H