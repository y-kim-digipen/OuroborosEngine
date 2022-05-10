#include <GL/glew.h>

#include <vulkan.h>
#include "Context.h"

#include <cassert>
#include <iostream>
#include <vector>


namespace Renderer
{

	void Context::SetupVulkan(int major, int minor, std::string application_name, std::string engine_name)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		if (!glfwVulkanSupported())
		{
			std::cout << "Vulkan is not supported!" << std::endl;
		}

		Context::vulkan_info.vk_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		Context::vulkan_info.vk_application_info.pApplicationName = application_name.c_str();
		Context::vulkan_info.vk_application_info.applicationVersion = VK_MAKE_API_VERSION(0, major, minor,0);
		Context::vulkan_info.vk_application_info.pEngineName = engine_name.c_str();
		Context::vulkan_info.vk_application_info.engineVersion = VK_MAKE_API_VERSION(0, major, minor, 0);
		Context::vulkan_info.vk_application_info.apiVersion = VK_MAKE_API_VERSION(0, major, minor, 0);


		Context::vulkan_info.vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		Context::vulkan_info.vk_instance_create_info.pNext = NULL;
		Context::vulkan_info.vk_instance_create_info.flags = 0;
		Context::vulkan_info.vk_instance_create_info.pApplicationInfo = &vulkan_info.vk_application_info;

	}

	Context::Context(GLFWwindow* glfwwindow) : window(glfwwindow) {}

	void Context::Init()
	{

		if (!is_vulkan)
		{
			glfwMakeContextCurrent(window);
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				std::cout << "Failed to init GLEW" << std::endl;
			}
			std::cout << "OpenGL Info :" << std::endl;
			std::cout << "OpenGL Version :" << glGetString(GL_VERSION) << std::endl;
			std::cout << "OpenGL Renderer:" << glGetString(GL_RENDERER) << std::endl;
			std::cout << "-------------------------------------------------" << std::endl;

		}
		else
		{
			//Vulkan
			uint32_t glfwExtensionCount;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");

			
			Context::vulkan_info.vk_instance_create_info.enabledExtensionCount = glfwExtensionCount;
			Context::vulkan_info.vk_instance_create_info.ppEnabledExtensionNames = glfwExtensions;
			Context::vulkan_info.vk_instance_create_info.enabledLayerCount = 0;


			VkResult result = vkCreateInstance(&vulkan_info.vk_instance_create_info, NULL, &instance);

			if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
			{
				std::cout <<
					"Cannot find a compatible Vulkan installable client "
					"driver (ICD). Please make sure your driver supports "
					"Vulkan before continuing. The call to vkCreateInstance failed." << std::endl;;
			}
			else if (result != VK_SUCCESS)
			{
				std::cout <<
					"The call to vkCreateInstance failed. Please make sure "
					"you have a Vulkan installable client driver (ICD) before "
					"continuing." << std::endl;
			}
			
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::cout << extensionCount << " extensions supported\n";

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			std::cout << "available extensions:\n";

			for (const auto& extension : extensions) {
				std::cout << '\t' << extension.extensionName << '\n';
			}

			uint32_t deviceCount = 0;
			result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

			assert(result == VK_SUCCESS);
			assert(deviceCount >= 1);

			std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
			result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
			assert(result == VK_SUCCESS);

			VkPhysicalDeviceProperties physicalProperties = {};

			for (uint32_t i = 0; i < deviceCount; i++)
			{
				vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalProperties);

				fprintf(stdout, "Device Name:    %s\n", physicalProperties.deviceName);
				fprintf(stdout, "Device Type:    %d\n", physicalProperties.deviceType);
				fprintf(stdout, "Driver Version: %d\n", physicalProperties.driverVersion);

				fprintf(stdout, "API Version:    %d.%d.%d\n",
					VK_VERSION_MAJOR(physicalProperties.apiVersion),
					VK_VERSION_MINOR(physicalProperties.apiVersion),
					VK_VERSION_PATCH(physicalProperties.apiVersion));
			}
			//std::cout << "Vulkan Info :" << std::endl;
			//std::cout << "Vulkan Version :" <<  << std::endl;
			//std::cout << "VUlkan Renderer:" <<  << std::endl;
			//std::cout << "-------------------------------------------------" << std::endl;


		}



		

	
	}

	void Context::SwapBuffer()
	{
		glfwSwapBuffers(window);
	}


}
