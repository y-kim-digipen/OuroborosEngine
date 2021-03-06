#include "image_asset_manager.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ranges>

#include "Graphics/vulkan/vulkan_context.h"
#include "../engine.h"
#include "Graphics/vulkan/vulkan_texture_manager.h"
namespace OE
{
    ImageAssetManager::~ImageAssetManager()
    {
        for (const auto& val : assets | std::views::values)
        {
            stbi_image_free(val.second.image);
        }
    }

    int ImageAssetManager::LoadAsset(const std::string& file_name)
    {
	    Asset::Image image{ nullptr, 0, 0 ,0, file_name };
        //int nrChannels;
        image.image = stbi_load(file_name.c_str(),
            &image.width, &image.height,
            &image.channel, STBI_rgb_alpha);

        if (!image.image)
        {
            std::cerr << "Failed to load image " + file_name << std::endl;
            return 1;
        }

        assets.try_emplace(file_name, std::make_pair(true, image));

        auto& texture_manager = Engine().Get().window->GetWindowData().RenderContextData->texture_manager_;
        texture_manager->AddTexture(file_name, image);
        return true;
    }

    int ImageAssetManager::UnloadAsset(const std::string& name)
    {
        auto& texture_manager = Engine().Get().window->GetWindowData().RenderContextData->texture_manager_;
        texture_manager->DeleteTexture(name);
        return true;
    }

    void ImageAssetManager::CleanUp()
    {
	    for (const auto& [key, val] : assets)
	    {
            UnloadAsset(key);
	    }
	    AssetManager<Asset::Image>::CleanUp();
    }
}
