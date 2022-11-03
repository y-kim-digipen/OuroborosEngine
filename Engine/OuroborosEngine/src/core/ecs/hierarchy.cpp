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
		auto parent = Engine::ecs_manager.GetEntity(new_parent_ID);
		if(parent_ID >= 0)
		{
			Engine::ecs_manager.GetEntity(parent_ID).hierarchy.DeleteChild(this_ID);
			parent_ID = -1;
		}

		if (parent.alive == false) { return false; }
		if (ContainsEntityInGroup(this, new_parent_ID) //is this entity contains parent?
			|| ContainsEntityInGroup(&Engine::ecs_manager.GetEntity(new_parent_ID).hierarchy, this_ID)) //is parent contains this
		{
			return false;
		}

		if (std::ranges::find(parent.hierarchy.child, this_ID) != parent.hierarchy.child.end()) //does parent already have this?
		{
			return false;
		}
		if (this_ID == parent_ID)
		{
			return false;
		}
		parent_ID = new_parent_ID;
		Engine::ecs_manager.GetEntity(parent_ID).hierarchy.child.push_back(this_ID);
		std::cout << "Parent" << parent_ID << "Child" << this_ID;
		return true;
	}
	return false;
}

bool OE::ECS::Hierarchy::HasParent() const
{
	return parent_ID != -1;
}

const std::list<OE::ecs_ID>& OE::ECS::Hierarchy::GetChild()
{
	return child;
}

bool OE::ECS::Hierarchy::HasChild(ecs_ID child_ID) const
{
	for (auto child : child)
	{
		if (child_ID == child)
		{
			return true;
		}
	}
	return false;
}

bool OE::ECS::Hierarchy::InsertChild(ecs_ID child_ID)
{
	if(HasChild(child_ID))
	{
		return false;
	}
	child.emplace_back(child_ID);
	return true;
}

bool OE::ECS::Hierarchy::DeleteChild(ecs_ID child_ID)
{
	if(!HasChild(child_ID))
	{
		return false;
	}
	Engine::ecs_manager.GetEntity(child_ID).hierarchy.parent_ID = -1;
	child.erase(std::find(child.begin(), child.end(), child_ID));
	return true;
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
