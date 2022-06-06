#pragma once
#include "ecs/ecs_base.h"
#include "ecs/components.h"
#include "ecs/system.h"

using ComponentList = OE::ECS::TypeList<Transform, Velocity, LifeTime, Mesh, BoolWrapper, Tag>;

using PhysicsSystem = OE::ECS::System<Transform, Velocity>;
using TestSystem = OE::ECS::System<BoolWrapper>;

using SystemList = OE::ECS::types::SystemList<PhysicsSystem, TestSystem>;

using Signature0 = OE::ECS::types::Signature<Transform, Velocity>;
using Signature1 = OE::ECS::types::Signature<LifeTime, Mesh>;
using Signature2 = OE::ECS::types::Signature<TestSystem, BoolWrapper>;

using MeshDrawSignature = OE::ECS::types::Signature<Transform, Mesh, Shader>;

using SignatureList = OE::ECS::types::SignatureList<Signature0, Signature1, Signature2,MeshDrawSignature>;

using OESettings = OE::ECS::_impl::Settings<ComponentList, SignatureList, SystemList>;

using ECS_Manager = OE::ECS::Manager<OESettings>;




static inline ECS_Manager ecs_manager;