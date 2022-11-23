#pragma once
#include <imgui.h>
#include <string>
#include <glm.hpp>
namespace OE
{
	namespace GUI
	{
		enum PanelFlags
		{
			NONE = 0,
			DESTROY_ON_EXIT = 1,
			EXIST_ONE = 1 << 1,
		};

		using PanelFlag = int32_t;

		constexpr size_t MAX_BUFFER_SIZE = 256;
		template<size_t SIZE>
		struct Buffer
		{
			Buffer(const std::string& s)
			{
				*this = s;
			}
			Buffer& operator =(const std::string& s)
			{
				assert(s.length() < SIZE);
				std::ranges::copy(s, data);
				data[s.length()] = '\0';
				return *this;
			}
			void Clear()
			{
				memset(data, 0, size);
				data[0] = '\0';
			}
			char data[SIZE];
			size_t size = SIZE;
		};

		using DefaultSizeBuffer = Buffer<MAX_BUFFER_SIZE>;

		namespace Colors
		{
			inline ImVec4 RED(1.0, 0.0, 0.0, 1.0);
			inline ImVec4 GREEN(0.0, 1.0, 0.0, 1.0);
			inline ImVec4 BLUE(0.0, 0.0, 1.0, 1.0);
			inline ImVec4 WHITE(1.0, 1.0, 1.0, 1.0);
			inline ImVec4 BLACK(0.0, 0.0, 0.0, 1.0);
		}
	}
}