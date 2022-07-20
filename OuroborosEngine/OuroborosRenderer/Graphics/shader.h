#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <memory>
#include "buffer.h"

#include <glm/matrix.hpp>

namespace Renderer {

	class Context;

	enum E_StageType {
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		MAX_VALUE
	};

	struct ShaderStage {
		E_StageType type = MAX_VALUE;
	};

	struct ShaderConfig {
		const char* name;
		ShaderStage stages[MAX_VALUE];
		uint32_t stage_count;

		void operator=(const ShaderConfig& config);
	};

	class Shader {
	public:
		Shader();
		Shader(const Shader& shader) = delete;
		Shader(Shader&& shader) = delete;
		virtual ~Shader() = 0 {};
		virtual void Init(ShaderConfig* config);
		virtual void Bind() = 0;
		virtual void Reload() = 0;
		virtual void ShutDown();

		virtual void* GetMemberVariable(const std::string& name);
		virtual void SetUniformValue(const char* name, void* data) {

			if (uniform_buffer_object->member_vars.find(name) != uniform_buffer_object->member_vars.end()) {
				uniform_buffer_object->UpdateData(name, data);
				//uniform_buffer_object->Bind();
			}
		}

		std::unique_ptr<UniformBuffer> uniform_buffer_object;
		bool reload_next_frame;
	protected:
		ShaderConfig config;
	};
}

/*

camera

1. shader uniform layout, member, type
	- read
	- 값을 지정

	projection, view 

global descriptor set = set 0

- projection
- view

push constant 
- model


*/


#endif // !SHADER_H

