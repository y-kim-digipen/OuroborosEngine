#ifndef SHADER_H
#define SHADER_H

#include <vector>

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
		virtual ~Shader() = 0 {};
		virtual void Init(ShaderConfig* config) = 0;
	private:
	};
}

#endif // !SHADER_H

