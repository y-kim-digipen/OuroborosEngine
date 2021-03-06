#include "shader.h"

namespace Renderer {
	Shader::Shader()
	{
		reload_next_frame = false;
	}

	void Shader::Init(ShaderConfig* config)
	{
		reload_next_frame = false;

		if (config != &this->config)
			this->config = *config;
	}

	void Shader::ShutDown()
	{
		uniform_buffer_object.release();
		uniform_buffer_object = nullptr;
	}

	void* Shader::GetMemberVariable(const std::string& name)
	{
		if (uniform_buffer_object->member_vars.find(name) != uniform_buffer_object->member_vars.end()) {
			return (char*)uniform_buffer_object->data + uniform_buffer_object->member_vars[name].offset;
		}

		return nullptr;
	}

	void ShaderConfig::operator=(const ShaderConfig& config)
	{
		name = config.name;
		
		for (uint32_t i = 0; i < MAX_VALUE; ++i) {
			stages[i] = config.stages[i];
		}

		stage_count = config.stage_count;
	}
}
