#include "opengl_shader.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <GL/glew.h>

namespace Renderer
{
	OpenglShader::OpenglShader()
	{

	}

	OpenglShader::~OpenglShader()
	{
		glDeleteProgram(shader_id);
		glDeleteShader(shader_id);
	}

	void OpenglShader::Init(ShaderConfig* config)
	{
		shader_id = glCreateProgram();

		GLint comp_result;
		std::vector<unsigned> shaders;
		std::string vert_path = config->name;

		std::ifstream vert_stream(vert_path);
		std::string vert_src;

		if (vert_stream.is_open())
		{
			vert_stream.seekg(0, std::ios::end);

			int size = static_cast<int>(vert_stream.tellg());

			vert_src.resize(size);

			vert_stream.seekg(0, std::ios::beg);

			vert_stream.read(vert_src.data(), size);
		}
		else
		{
			throw std::runtime_error("Cannot open vertex shader source file!");
		}
		vert_stream.close();
	
		const char* vert_cstring = vert_src.c_str();
		unsigned int vert_id = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert_id, 1, &vert_cstring, nullptr);
		glCompileShader(vert_id);

		glGetShaderiv(vert_id, GL_COMPILE_STATUS, &comp_result);
		if (GL_FALSE == comp_result)
		{
			std::cout << "vertex shader compilation failed\n";
		}

		shaders.emplace_back(vert_id);

		std::string frag_path = config->name;
		std::ifstream frag_stream(frag_path);

		std::string frag_src;

		if (frag_stream.is_open())
		{
			frag_stream.seekg(0, std::ios::end);

			int size = static_cast<int>(frag_stream.tellg());

			frag_src.resize(size);

			frag_stream.seekg(0, std::ios::beg);

			frag_stream.read(frag_src.data(), size);
		}
		else
		{
			throw std::runtime_error("Cannot open fragment shader source file!");
		}
		frag_stream.close();
		const char* frag_cstring = frag_src.c_str();
		unsigned int frag_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_id, 1, &frag_cstring, nullptr);
		glCompileShader(frag_id);

		glGetShaderiv(frag_id, GL_COMPILE_STATUS, &comp_result);
		if (GL_FALSE == comp_result)
		{
			std::cout << "fragment shader compilation failed\n";
		}

		shaders.emplace_back(frag_id);

		LinkShader(shaders);
	}

	void OpenglShader::Bind()
	{

	}

	void OpenglShader::LinkShader(std::vector<unsigned> shader_ids)
	{
		for (auto& shader : shader_ids)
		{
			glAttachShader(shader_id, shader);
		}


		glLinkProgram(shader_id);
		GLint lnk_log;
		glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &lnk_log);
		char* error_log = new char[lnk_log];
		error_log[0] = '\0';
		for (auto& shader : shader_ids)
		{
			glDetachShader(shader_id, shader);
			glDeleteShader(shader);
		}
		GLint lnk_status;
		glGetProgramiv(shader_id, GL_LINK_STATUS, &lnk_status);
		if (GL_FALSE == lnk_status)
		{
			std::cerr << "Failed to link shader program\n";
			glGetProgramInfoLog(shader_id, lnk_log, NULL, error_log);
			std::cout << error_log << std::endl;
		}
		if (lnk_log > 0)
			delete[] error_log;

		glValidateProgram(shader_id);
		GLint is_validate = 0;
		glGetProgramiv(shader_id, GL_VALIDATE_STATUS, &is_validate);
		if (is_validate == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
			std::vector<char> error(log_length);
			glGetProgramInfoLog(shader_id, log_length, nullptr, error.data());
			throw std::runtime_error(std::string(error.begin(), error.end()));
		}

	}
}
