#pragma once
#include <type_traits>

namespace OE
{
    using ecs_ID = int16_t;
    static_assert(sizeof(ecs_ID) == 2 && std::is_signed_v<ecs_ID>,
        "ID type mismatch");

    constexpr ecs_ID MAX_ENTITIES = 1000;
    static_assert(MAX_ENTITIES >= 0, 
        "Number of maximum entities must be smaller than its' type's maximum amount");

    #define GET_VARIABLE_NAME(Variable) (#Variable)
	#define GET_CLASS_NAME(CLASS) ("CLASS")

    enum class Status { NONE, CREATED, INIT, UPDATE, DESTROYED, CLEANUP, COUNT };
}
