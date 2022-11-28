#pragma once
#include <array>
#include <string>

namespace Shared
{
    enum class MaterialMode
    {
        NONE_TEXTURE, TEXTURE,
    };

    enum PBR_TEXTURE_TYPES
    {
        ALBEDO, NORMAL, METALLIC_ROUGHNESS, METALLIC, ROUGHNESS, AO, EMISSIVE, COUNT,
    };

    struct PBR_NONE_TEXTURE
    {
        float metallic, roughness, ao;
        glm::vec3 albedo;

        PBR_NONE_TEXTURE() : metallic(0.f), roughness(0.f), ao(0.f), albedo(glm::vec3(0.f)) {}
    };
    struct PBR_TEXTURE
    {
        PBR_TEXTURE() : is_smoothness(false), use_combined_metallic_roughness(false)
        {
	        for (auto& element : texture_names)
	        {
                element = std::string{};
	        }
        }
        std::array<std::string, PBR_TEXTURE_TYPES::COUNT> texture_names = {""};

        bool is_smoothness;
        bool use_combined_metallic_roughness;
    };
    union PBR_Structure_Internal
    {
        PBR_NONE_TEXTURE* non_texture;
        PBR_TEXTURE     * texture;
    };

    struct PBR_Structure
    {
    private:
        MaterialMode mode;
    public:
        PBR_Structure_Internal structure{};
        PBR_Structure() : PBR_Structure(MaterialMode::NONE_TEXTURE) {}
        PBR_Structure(MaterialMode mode) : mode(mode)
        {
            switch (mode)
            {
            case MaterialMode::NONE_TEXTURE:
                structure.non_texture   = new PBR_NONE_TEXTURE;
                break;
            case MaterialMode::TEXTURE:
                structure.texture       = new PBR_TEXTURE;
                break;
            }
        }
        ~PBR_Structure()
        {
            switch (mode)
            {
            case MaterialMode::NONE_TEXTURE:
                delete structure.non_texture;
                break;
            case MaterialMode::TEXTURE:
                delete structure.texture;
                break;
            }
        }

        PBR_Structure& operator = (const PBR_Structure& other)
        {
            if (other.mode != mode)
            {
                switch (mode)
                {
                case MaterialMode::NONE_TEXTURE:
                    delete structure.non_texture;
                    structure.texture = new PBR_TEXTURE;
                    *structure.texture = *other.structure.texture;
                    break;
                case MaterialMode::TEXTURE:
                    delete structure.texture;
                    structure.non_texture = new PBR_NONE_TEXTURE;
                    *structure.non_texture = *other.structure.non_texture;
                    break;
                }
                mode = other.mode;
            }
            return *this;
        }

        PBR_Structure& operator = (PBR_Structure&& other)
        {
            if (other.mode != mode)
            {
                switch (mode)
                {
                case MaterialMode::NONE_TEXTURE:

                    delete structure.non_texture;
                    structure.texture = other.structure.texture;
                    other.structure.texture = nullptr;
                    break;
                case MaterialMode::TEXTURE:
                    delete structure.texture;
                    structure.non_texture = other.structure.non_texture;
                    other.structure.non_texture = nullptr;
                    break;
                }
                mode = other.mode;
            }
            return *this;
        }

        MaterialMode GetMode() const
        {
            return mode;
        }
        
        void SwitchMode(MaterialMode new_mode)
        {
	        if(mode != new_mode)
	        {
                switch (mode)
                {
                case MaterialMode::NONE_TEXTURE:
                    delete structure.non_texture;
                    structure.texture = new PBR_TEXTURE;
                    break;
                case MaterialMode::TEXTURE:
                    delete structure.texture;
                    structure.non_texture = new PBR_NONE_TEXTURE;
                    break;
                }
                mode = new_mode;
	        }
        }
    };
}