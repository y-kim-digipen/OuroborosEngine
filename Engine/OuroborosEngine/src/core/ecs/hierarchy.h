#pragma once
#include <list>

#include "../engine_definitions.h"

namespace OE
{
	namespace ECS
	{
		class Hierarchy
		{
		public:
			bool SetParent(ecs_ID parent_ID);
			bool HasParent() const;

			const std::list<ecs_ID>& GetChild();
		private:
			bool ContainsEntityInGroup(Hierarchy* parent_hierarchy, ecs_ID finding_ID);
			ecs_ID GetEntityID() const;
			ecs_ID parent_ID = -1;
			std::list<ecs_ID> child;
		};
	}

}
