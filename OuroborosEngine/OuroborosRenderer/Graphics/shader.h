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
	};

	class Shader {
	public:
		Shader();
		Shader(const Shader& shader) = delete;
		Shader(Shader&& shader) = delete;
		virtual ~Shader() = 0 {};
		virtual void Init(ShaderConfig* config) = 0;
		virtual void Bind() = 0;
		virtual void SetUniformValue(const char* name, void* data) {

			for (const auto& ubo : uniform_buffer_objects) {
				if (ubo->member_vars.find(name) != ubo->member_vars.end()) {
					ubo->UpdateData(name, data);
				}
			}

		}

		virtual void BindObjectData(const glm::mat4& model) = 0;
		

		/*
		global_ubo[0]
		layout(set = 0, binding = 0) {
			mat4 projection;
			mat4 view;
		}
	
		global_ubo[1]
		layout(set = 0, binding = 1) {
			vec3 other_value;
			vec4 other_other_value;
		}


		DescriptorSet 0 -> global
			DescriptorSet -> bindings collection
			binding = uniform buffer


		std::vector<std::unique_ptr<UniformBuffer>> global_ubo;
		std::vector< std::unique_ptr<UniformBuffer>> renderpass_ubo;
		std::vector < std::unique_ptr<UniformBuffer>> material_ubo;
		std::vector<std::unique_ptr<UniformBuffer>> object_ubo;
		*/

		std::vector<std::unique_ptr<UniformBuffer>> uniform_buffer_objects;
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

