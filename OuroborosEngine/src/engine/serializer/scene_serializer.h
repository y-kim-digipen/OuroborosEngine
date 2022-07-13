#pragma once
#include <string>
#include <yaml-cpp/yaml.h>
namespace OE
{
	class SceneSerializer
	{
	public:
		void Init();
		void SerializeScene(const std::string& path);
		void DeserializeScene(const std::string& file_name);
		void CleanUp();
	private:
		void EmitToFile(const std::string& file_name);
		YAML::Emitter* emitter = nullptr;
	};
}
