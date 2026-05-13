#include "rendering/renderers/QuadTreeRenderer.hpp"
#include "game/GameContext.hpp"
#include "rendering/Renderer.hpp"

#include "Global.hpp"

#include "model/Mesh.hpp"
#include "glm/ext/matrix_transform.hpp"

QuadTreeRenderer::QuadTreeRenderer(Renderer& renderer) : renderer(renderer), quadTree(global.gameCtx.qt) 
{
    quads.resize(quadTree.maxDepth);
    quadInstances.resize(quadTree.maxDepth);

    GameContext::AABB aabb = quadTree.bounds();
    aabb.maxX = aabb.maxX - aabb.minX;
    aabb.maxY = aabb.maxY - aabb.minY;
    aabb.minX = 0;
    aabb.minY = 0;

    for(int i = 0; i < quadTree.maxDepth; i++)
    {
        Mesh mesh;

        MeshTools::Quad quad;
        quad.x2 = static_cast<float>(aabb.maxX);
        quad.y2 = static_cast<float>(aabb.maxY);
        MeshTools::createQuad(&mesh, quad);
        
        quads[i].updateBuffers(mesh.vertices, mesh.indices);

        aabb.maxX /= 2;
        aabb.maxY /= 2;
    }
    
    global.gameCtx.buildQTree();
    for(const auto &node : quadTree.nodes())
    {
        const GameContext::AABB &aabb = node.bounds;
        auto& instances = quadInstances[static_cast<u64>(node.depth-1)];

        instances.emplace_back();
        instances.back().model = glm::translate(glm::mat4(1.0f), glm::vec3(aabb.minX, aabb.minY, 0));
    }
}

void QuadTreeRenderer::submit(RenderQueue& queue)
{
    
    for(int i = 0; i < quadTree.maxDepth; i++)
    {
        auto& instances = quadInstances[i];
        if(instances.empty()) break;

        queue.batchQueue.push_back(
            {
                &quads[i],
                renderer.vkWireframeRenderSystem.getDefaultMaterial(),
                instances.data(),
                static_cast<u32>(instances.size())
            }
        );
    }
}
