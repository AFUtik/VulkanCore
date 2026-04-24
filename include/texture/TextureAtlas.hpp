#pragma once

#include <vector>
#include <unordered_map>

#include <stb_rect_pack.h>

#include "Texture.hpp"

struct TextureUV
{
    float u1, v1, u2, v2;
};

struct TextureAtlas 
{
    std::vector<TextureUV> uvs;
    std::unordered_map<std::string, uint32_t> uvs_map;

    Texture texture;
};

struct AtlasBuilder 
{
    void build(TextureAtlas* atlas);

    void pack(
        TextureAtlas* atlas,
        Texture* texture_ptr,
        const std::string& tag = "");

    void reserve(int size);
    void clear();

    inline void setPadding(int padding)
    {
        this->padding = padding;
    }

    inline void setMultithreading(bool flag) 
    {
        enableMultithreading = flag;
    }
private:
    stbrp_context context;
    std::vector<stbrp_node> nodes;
    std::vector<stbrp_rect> rects;
    std::vector<Texture*>   images_ptrs;

    bool enableMultithreading = false;
    int padding = 0;
    int index = 0;
};