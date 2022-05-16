#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include <unordered_map>

#include "../shader.h"
namespace Renderer
{

	class OpenglShader : public Shader
	{
	public:
		OpenglShader();
		~OpenglShader() override;
		void Init(ShaderConfig* config) override;


	private:
		unsigned int shader_id;
		void LinkShader(std::vector<unsigned> ids);
		std::unordered_map<std::string, unsigned> uniform_ids;
	};






}





#endif
