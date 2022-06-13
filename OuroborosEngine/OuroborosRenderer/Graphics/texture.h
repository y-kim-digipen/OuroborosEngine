#ifndef TEXTURE_H
#define TEXTURE_H
#include <assets.h>

namespace Renderer
{
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void Bind() = 0;
		virtual void UploadData(const Asset::Image& data);
	};

}



#endif !TEXTURE_H