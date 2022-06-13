#include <GL/glew.h>

#include "Context.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

#include "shader.h"

namespace Renderer
{
	Context::Context(GLFWwindow* glfwwindow) : window(glfwwindow) {}


	void Context::InitGlobalData()
	{
		light_data.num_of_lights = 0;
	}

	void Context::UpdateGlobalData()
	{
	}

	void Context::SwapBuffer()
	{
		glfwSwapBuffers(window);
	}
	int Context::BeginFrame()
	{
		return 0;
	}

	int Context::EndFrame()
	{
		return 0;
	}

	void Context::AddDrawQueue(TransformComponent* transform, MaterialComponent* material, MeshComponent* mesh,
		ShaderComponent* shader)
	{
		draw_queue.push({ transform,material, mesh, shader });
	}

	int Context::AddLight(uint32_t entity_id, Asset::LightData* light_component)
	{
		// if there is slot left
		if (light_data.num_of_lights < max_num_lights)
		{
			light_map[entity_id] = light_data.num_of_lights;
			memcpy_s(&light_data.lights[light_data.num_of_lights], sizeof(Asset::LightData), light_component, sizeof(Asset::LightData));
			++light_data.num_of_lights;
			return 0;
		}

		std::cout << "Light Slots are full, remove another light object to add light" << std::endl;
		return -1;
	}

	void Context::RemoveLight(uint32_t entity_id)
	{
		if (light_map.find(entity_id) == light_map.end())
		{
			std::cout << "light don't exists!" << std::endl;
			return;
		}

		uint32_t index = light_map[entity_id];

		// if its not last element, swap with last element 
		if (index != (light_data.num_of_lights - 1))
		{
			std::swap(light_data.lights[light_data.num_of_lights - 1], light_data.lights[index]);

			// first = entity_id, second = light_array_index
			// find light that is currently binded to last element and swap 'last index' to 'index'

			for (auto& pair : light_map) 
			{
				if (pair.second == light_data.num_of_lights - 1) 
				{
					pair.second = index;
					break;
				}
			}

			light_map[entity_id] = light_data.num_of_lights - 1;
		}

		--light_data.num_of_lights;
	}

	void Context::UpdateLight(uint32_t entity_id, Asset::LightData* light_component)
	{
		if (auto iter = light_map.find(entity_id); iter != light_map.end())
		{
			//global_data.lights[light_map[entity_id]] = *light_component;
			memcpy_s(&light_data.lights[iter->second], sizeof(Asset::LightData), light_component, sizeof(Asset::LightData));
		}
	}


	Context::~Context()
	{
	}
}
