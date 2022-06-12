#include "image_asset_manager.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ranges>
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
	    Asset::Image image{ nullptr, 0, 0 };
        std::string file_path = "image/";
        file_path.append(file_name);
        int nrChannels;
        image.image = stbi_load(file_path.c_str(),
            &image.width, &image.height,
            &nrChannels, 0);
        if (!image.image)
        {
            std::cerr << "Failed to load image " + file_name << std::endl;
            return 1;
        }
        assets.try_emplace(file_name, std::make_pair(true, image));
        return true;
    }


}
