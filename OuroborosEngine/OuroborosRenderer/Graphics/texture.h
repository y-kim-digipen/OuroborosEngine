#ifndef TEXTURE_H
#define TEXTURE_H

namespace Asset
{
	struct Image;
}

namespace Renderer
{
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void Bind() = 0;
		virtual void UploadData(const Asset::Image& data) = 0;
		virtual void Cleanup() = 0;
	};
}


#endif !TEXTURE_H