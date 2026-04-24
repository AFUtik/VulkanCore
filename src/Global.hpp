#pragma once

#include "rendering/Renderer.hpp"

#include "texture/TextureAtlas.hpp"
#include "texture/Tileset.hpp"

struct Global
{
    AtlasBuilder atlasBuilder;

    std::vector<std::unique_ptr<TextureAtlas>> atlases;
    std::unordered_map<std::string, uint32_t> atlasMap;

    std::unique_ptr<Renderer> renderer;

    void shutdown();

    Global();
};

static Global global;

const std::string RESOURCE_PATH = "C:\\cplusplus\\VulkanRender\\VulkanRender\\resources\\";