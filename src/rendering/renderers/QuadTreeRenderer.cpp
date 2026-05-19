#include "rendering/renderers/QuadTreeRenderer.hpp"
#include "rendering/systems/BaseRenderSystem.hpp"
#include "rendering/Renderer.hpp"

#include "Global.hpp"

#include "glm/ext/matrix_transform.hpp"
#include <iostream>

class MeshTools {
public:
    struct Quad {
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
        float u1 = 0.0f, v1 = 0.0f, u2 = 0.0f, v2 = 0.0f;
        float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    };

    static void createQuad(std::vector<Vertex>& vertices, std::vector<u32>& indices, Quad& quad)
    {
        vertices.push_back({quad.x2, quad.y2, 0.0f, quad.u2, quad.v1, quad.r, quad.g, quad.b, quad.a});
        vertices.push_back({quad.x2, quad.y1, 0.0f, quad.u2, quad.v2, quad.r, quad.g, quad.b, quad.a});
        vertices.push_back({quad.x1, quad.y1, 0.0f, quad.u1, quad.v2, quad.r, quad.g, quad.b, quad.a});
        vertices.push_back({quad.x1, quad.y2, 0.0f, quad.u1, quad.v1, quad.r, quad.g, quad.b, quad.a});

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);
    }
};

QuadTreeRenderer::QuadTreeRenderer(Renderer& renderer) : renderer(renderer), quadTree(global.gameCtx.qt) 
{
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
    MeshTools::createQuad(vertices, indices, quad);

    vkQuad.updateBuffers(std::as_bytes(std::span(vertices)), indices);
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
            &vkQuad,
            renderer.vkWireframeRenderSystem->getDefaultMaterial(),
            quadInstances.data(),
            static_cast<u32>(quadInstances.size())
        }
    );
}
