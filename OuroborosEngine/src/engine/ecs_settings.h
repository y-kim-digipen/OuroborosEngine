#pragma once
#include "ecs/ecs_base.h"
#include "ecs/components.h"

using ComponentList = OE::ECS::TypeList<Transform, Velocity, LifeTime, Mesh, BoolWrapper, Tag>;

using Signature0 = OE::ECS::types::Signature<Transform, Velocity>;
using Signature1 = OE::ECS::types::Signature<LifeTime, Mesh>;
using Signature2 = OE::ECS::types::Signature<BoolWrapper>;

using SignatureList = OE::ECS::types::SignatureList<Signature0, Signature1, Signature2>;

using OESettings = OE::ECS::_impl::Settings<ComponentList, SignatureList>;

using ECS_Manager = OE::ECS::Manager<OESettings>;

static inline ECS_Manager ecs_manager;