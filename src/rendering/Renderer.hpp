#pragma once 

#include "../vk/Renderer.hpp"
#include "../vk/Material.hpp"
#include "../vk/RenderTarget.hpp"

#include "RenderSystem.hpp"
#include "GlobalRenderSystem.hpp"


#include <unordered_map>

struct Renderer 
{
    Renderer();
    ~Renderer() {};

    myvk::Renderer vkRenderer;
    myvk::GlobalRenderSystem vkRenderSystem;

    myvk::Material vkDefaultMat; 

    myvk::MaterialHandle materialInstance();
private:
    void createDefaultMaterial();
};