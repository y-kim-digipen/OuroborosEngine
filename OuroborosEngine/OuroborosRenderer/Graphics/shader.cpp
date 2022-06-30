#include "shader.h"

namespace Renderer {
	Shader::Shader()
	{
	}

	void* Shader::GetMemberVariable(const std::string& name)
	{
		if (uniform_buffer_object->member_vars.find(name) != uniform_buffer_object->member_vars.end()) {
			return (char*)uniform_buffer_object->data + uniform_buffer_object->member_vars[name].offset;
		}

		return nullptr;
	}
}
