#include "hierarchy.h"
#include "../engine.h"
#include "ecs_base.h"

bool OE::ECS::Hierarchy::SetParent(ecs_ID new_parent_ID)
{
	const ecs_ID this_ID = GetEntityID();

	if (new_parent_ID == -1)
	{
		if(parent_ID < 0)
		{
			return false;
		}
		auto& old_parent = Engine::ecs_manager.GetEntity(parent_ID);
		auto to_delete = std::ranges::find(old_parent.hierarchy.child, this_ID);
		if(to_delete != old_parent.hierarchy.child.end())
		{
			old_parent.hierarchy.child.erase(to_delete);
			parent_ID = -1;
		}
	}
	else
	{
		auto entity = Engine::ecs_manager.GetEntity(new_parent_ID);


		if (entity.alive == false) { return false; }
		if (ContainsEntityInGroup(this, new_parent_ID)
			|| ContainsEntityInGroup(&Engine::ecs_manager.GetEntity(new_parent_ID).hierarchy, new_parent_ID))
		{
			return false;
		}

		if (std::ranges::find(entity.hierarchy.child, this_ID) != entity.hierarchy.child.end())
		{
			return false;
		}
		if (this_ID == parent_ID)
		{
			return false;
		}
		parent_ID = new_parent_ID;
		Engine::ecs_manager.GetEntity(parent_ID).hierarchy.child.push_back(this_ID);
		return true;
	}
}

bool OE::ECS::Hierarchy::HasParent() const
{
	return parent_ID != -1;
}

const std::list<OE::ecs_ID>& OE::ECS::Hierarchy::GetChild()
{
	return child;
}

bool OE::ECS::Hierarchy::ContainsEntityInGroup(Hierarchy* parent_hierarchy, ecs_ID finding_ID)
{
	ecs_ID my_ID = GetEntityID();
	if(child.empty())
	{
		return false;
	}
	for (auto ecs_ID : child)
	{
		if(ecs_ID == finding_ID)
		{
			return true;
		}
	}

	for (auto ecs_ID : child)
	{
		if (bool res = ContainsEntityInGroup(&Engine::ecs_manager.GetEntity(ecs_ID).hierarchy, finding_ID))
		{
			return true;
		}
	}
	return false;
}

OE::ecs_ID OE::ECS::Hierarchy::GetEntityID() const
{
	//using EntityType = decltype(std::declval<Engine>().ecs_manager)::Entity;
	constexpr size_t entity_size = sizeof(ECS_Manager::Entity);

	char* entity_start_address = (char*)&Engine::ecs_manager.GetEntity(0);
	char* this_memory_address = (char*)(this);

	const size_t address_distance = this_memory_address - entity_start_address;

	const auto entity_ID = static_cast<ecs_ID>(address_distance / entity_size);

	return entity_ID;
}
