#pragma once
#include "ecs/ecs_base.h"
#include "ecs/components.h"
#include "ecs/system.h"
#include "modules/master_asset_manager.h"
#include "modules/image_asset_manager.h"
#include "modules/mesh_asset_manager.h"
#include "modules/shader_asset_manager.h"
#include "modules/script_asset_manager.h"
#include "modules/material_asset_manager.h"

using ComponentList = OE::ECS::TypeList<
	TransformComponent,
	MeshComponent,
	ShaderComponent,
	MaterialComponent,
	TagComponent,
	LightComponent,
	ScriptComponent
>;

using DrawSystem		= OE::ECS::System<TransformComponent, ShaderComponent, MaterialComponent, MeshComponent>;
using LightSystem		= OE::ECS::System<ShaderComponent, LightComponent, TransformComponent, MaterialComponent>;
using ScriptingSystem	= OE::ECS::System<ScriptComponent>;
using SystemList = OE::ECS::types::SystemList<DrawSystem, LightSystem, ScriptingSystem>;

//using MeshDrawSignature = OE::ECS::types::Signature<TransformComponent, MeshComponent, ShaderComponent>;

using SignatureList = OE::ECS::types::SignatureList</*MeshDrawSignature*/>;

using OESettings = OE::ECS::_impl::Settings<ComponentList, SignatureList, SystemList>;

using ECS_Manager = OE::ECS::Manager<OESettings>;

using Asset_Manager = OE::MasterAssetManager<OE::ImageAssetManager, OE::MeshAssetManager, OE::ShaderAssetManager, OE::ScriptAssetManager, OE::MaterialAssetManager>;
