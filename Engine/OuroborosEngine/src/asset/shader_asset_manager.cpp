#include "shader_asset_manager.h"

#include "shader_manager.h"
#include "Context.h"

#include "../core/engine.h"

int OE::ShaderAssetManager::LoadAsset(const std::string& file_name)
{
    Asset::Shader shader;
    shader.name = file_name;

	//TODO: make configurable
    Renderer::ShaderConfig shader_config{
        file_name.c_str(),
        {
            Renderer::E_StageType::VERTEX_SHADER,
            Renderer::E_StageType::FRAGMENT_SHADER
        },
        2
        ,true
    };

    (Engine().Get().window.get()->GetWindowData().RenderContextData.get())->shader_manager->AddShader(&shader_config);
    assets[file_name] = std::make_pair(true, shader);
   
    return 0;
}

int OE::ShaderAssetManager::UnloadAsset(const std::string& name)
{
    //TODO: make configurable
    Renderer::ShaderConfig shader_config{
        name.c_str(),
        {
            Renderer::E_StageType::VERTEX_SHADER,
            Renderer::E_StageType::FRAGMENT_SHADER
        },
        2
    };

    (Engine().Get().window.get()->GetWindowData().RenderContextData.get())->shader_manager->DeleteShader(&shader_config);
    AssetManager::UnloadAsset(name);
    return 0;
}
