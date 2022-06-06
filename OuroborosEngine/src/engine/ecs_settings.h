#pragma once
#include "ecs/ecs_base.h"
#include "ecs/components.h"
#include "ecs/system.h"
#include "common.h"

using ComponentList = OE::ECS::TypeList<TransformComponent, VelocityComponent, LifeTimeComponent, MeshComponent, BoolWrapperComponent, ShaderComponent,MaterialComponent, TagComponent>;

using PhysicsSystem = OE::ECS::System<TransformComponent, VelocityComponent>;
using TestSystem = OE::ECS::System<BoolWrapperComponent>;
using DrawSystem = OE::ECS::System<TransformComponent, ShaderComponent, MaterialComponent, MeshComponent>;


using SystemList = OE::ECS::types::SystemList<PhysicsSystem, TestSystem, DrawSystem>;

using Signature0 = OE::ECS::types::Signature<TransformComponent, VelocityComponent>;
using Signature1 = OE::ECS::types::Signature<LifeTimeComponent, MeshComponent>;
using Signature2 = OE::ECS::types::Signature<TestSystem, BoolWrapperComponent>;

using MeshDrawSignature = OE::ECS::types::Signature<TransformComponent, MeshComponent,ShaderComponent>;

using SignatureList = OE::ECS::types::SignatureList<Signature0, Signature1, Signature2, MeshDrawSignature>;

using OESettings = OE::ECS::_impl::Settings<ComponentList, SignatureList, SystemList>;

using ECS_Manager = OE::ECS::Manager<OESettings>;


static inline ECS_Manager ecs_manager;