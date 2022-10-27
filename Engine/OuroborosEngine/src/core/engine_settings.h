#pragma once
#include <common/components.h>
#include "ecs/ecs_base.h"

#include "ecs/system.h"
#include "../asset/master_asset_manager.h"
#include "../asset/image_asset_manager.h"
#include "../asset/mesh_asset_manager.h"
#include "../asset/shader_asset_manager.h"
#include "../asset/script_asset_manager.h"
#include "../asset/material_asset_manager.h"

using ComponentList = OE::ECS::TypeList<
	TransformComponent,
	MeshComponent,
	ShaderComponent,
	MaterialComponent,
	TagComponent,
	LightComponent,
	ScriptComponent,
	CameraComponent
>;

using DrawSystem		= OE::ECS::System<TransformComponent, ShaderComponent, MaterialComponent, MeshComponent>;
using LightSystem		= OE::ECS::System<ShaderComponent, LightComponent, TransformComponent, MaterialComponent>;
using ScriptingSystem	= OE::ECS::System<ScriptComponent>;
using SystemList = OE::ECS::types::SystemList<DrawSystem, LightSystem, ScriptingSystem>;

using CameraTransformSyncSignature = OE::ECS::types::Signature<TransformComponent, CameraComponent>;

using SignatureList = OE::ECS::types::SignatureList<CameraTransformSyncSignature>;

using OESettings = OE::ECS::_impl::Settings<ComponentList, SignatureList, SystemList>;

using ECS_Manager = OE::ECS::Manager<OESettings>;

using Asset_Manager = OE::MasterAssetManager<OE::ImageAssetManager, OE::MeshAssetManager, OE::ShaderAssetManager, OE::ScriptAssetManager, OE::MaterialAssetManager>;
