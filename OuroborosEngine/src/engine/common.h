#pragma once
#include <cstdint>
#include <string>

using entity_ID		= int16_t;
using component_ID	= entity_ID;
using system_ID		= component_ID;

static_assert(sizeof(entity_ID)		== 2 && std::is_signed_v<entity_ID>, 
				"ID type mismatch");
static_assert(sizeof(component_ID)	== 2 && std::is_signed_v<entity_ID>, 
				"ID type mismatch");
static_assert(sizeof(system_ID)		== 2 && std::is_signed_v<entity_ID>, 
				"ID type mismatch");