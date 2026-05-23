#include "rendering/renderers/QuadTreeRenderer.hpp"
#include "rendering/systems/BaseRenderSystem.hpp"
#include "rendering/Renderer.hpp"

#include "Global.hpp"
#include "MeshTools.hpp"

#include "glm/ext/matrix_transform.hpp"

#include "gfx/gfx.hpp"
#include "gfx/VulkanBackend.hpp"

QuadTreeRenderer::QuadTreeRenderer(Renderer& renderer) : renderer(renderer), quadTree(global.gameCtx.qt) 
{
    auto gfx = reinterpret_cast<gfx_vk::VulkanRenderDevice*>(gfx::gfx.iRenderDevice.get());
	gfx->meshResource.objects_.type_size = sizeof(BaseMesh);

    GameContext::AABB aabb = quadTree.bounds();
    aabb.maxX = aabb.maxX - aabb.minX;
    aabb.maxY = aabb.maxY - aabb.minY;
    aabb.minX = 0;
    aabb.minY = 0;

    MeshTools::Quad quad{};
    quad.x2 = static_cast<float>(1.0f);
    quad.y2 = static_cast<float>(1.0f);

    std::vector<Vertex> vertices; 
    std::vector<u32> indices;
    MeshTools::generate(vertices, indices, quad);
    
    vkQuad = gfx->meshResource.Create<BaseMesh>();
    vkQuad->updateVertexBuffer(vertices.data(), vertices.size());
    vkQuad->updateIndexBuffer(indices.data(), indices.size());
}

void QuadTreeRenderer::submit(RenderQueue& queue)
{
    quadInstances.clear();
    for(const auto &node : quadTree.nodes())
    {
        assert(node.depth <= quadTree.maxDepth);

        const GameContext::AABB &aabb_n = node.bounds;

        quadInstances.emplace_back();
        glm::vec3 size(
            aabb_n.maxX - aabb_n.minX,
            aabb_n.maxY - aabb_n.minY,
            1.0f
        );

        quadInstances.back().model =
            glm::translate(glm::mat4(1.0f), glm::vec3(aabb_n.minX, aabb_n.minY, 0.0f)) *
            glm::scale(glm::mat4(1.0f), size);
    }

    queue.batchQueue.push_back(
        {
            vkQuad.Get(),
            renderer.baseRenderSystem->getDefaultMaterial(),
            quadInstances.data(),
            static_cast<u32>(quadInstances.size())
        }
    );
}
