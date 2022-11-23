#pragma once
#include "src/engine/common.h"
#include "src/engine/custom_assert.h"

namespace OE
{
	class ID_Generator
	{
		static ecs_ID GenerateID()
		{
			static ecs_ID counter = 0;
			OE_Assert(counter == INT16_MAX, "ID encountered maximum size of ID generator");
			return counter++;
		}
	};


	struct entity
	{
		ecs_ID ID;
	};

	struct Component
	{
		ecs_ID parent_ID;
	};
}
