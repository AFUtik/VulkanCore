#include "texture/TextureAtlas.hpp"
#include "texture/Texture.hpp"

void AtlasBuilder::build(TextureAtlas* atlas) 
{
    atlas->uvs.resize(nodes.size());

    const int width  = atlas->texture.width;
    const int height = atlas->texture.height;

    stbrp_init_target(&context, width, height, nodes.data(), nodes.size());
    stbrp_pack_rects (&context, rects.data(), rects.size());

    for (int i = 0; i < index; i++) 
    {
        Texture* texture = images_ptrs[i];
        auto& rect = rects[i];

        TextureUtils::transfer(
            &atlas->texture, 
            texture, 
            rect.x + padding, 
            rect.y + padding);

        TextureUtils::extrudePadding(
            &atlas->texture,
            rect.x + padding,
            rect.y + padding,
            texture->width,
            texture->height, 
            padding);
        
        auto& region = atlas->uvs[i];
        region.u1 = (float)(rect.x + padding) / width;
        region.v1 = (float)(rect.y + padding) / height;
        region.u2 = (float)(rect.x + padding + texture->width)  / width;
        region.v2 = (float)(rect.y + padding + texture->height) / height;
    }
    index = 0;
    clear();
}

void AtlasBuilder::pack(
    TextureAtlas* atlas,
    Texture* texture, 
    const std::string& tag) 
{   
    if(!tag.empty()) atlas->uvs_map.emplace(tag, index);

    rects.push_back({stbrp_rect{index, texture->width + padding*2, texture->height + padding*2}});
    images_ptrs.push_back(texture);

    index++;
}

void AtlasBuilder::reserve(int size) 
{
    nodes.resize(size);
}

void AtlasBuilder::clear() 
{
    nodes.clear();
    rects.clear();
    images_ptrs.clear();
}