#pragma once

#include "texture/TextureAtlas.hpp"
#include "texture/Tileset.hpp"

const std::string RESOURCES_PATH = "C:\\cplusplus\\VulkanRender\\VulkanRender\\resources\\";

struct Assets
{ 
    AtlasBuilder atlasBuilder;

    std::vector<std::unique_ptr<TextureAtlas>> atlases;
    std::unordered_map<std::string, TextureAtlas*>  atlasMap;

    std::vector<std::unique_ptr<Tileset>>     tilesets;
    std::unordered_map<std::string, Tileset*> tilesetMap;

    void load();
    void reload(); 
};