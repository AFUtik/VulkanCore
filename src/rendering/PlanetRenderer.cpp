#include "rendering/renderers/PlanetRenderer.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/RenderQueue.hpp"

#include "model/Mesh.hpp"

#include "Color.hpp"
#include <numbers>

#include <glm/ext.hpp>

PlanetRenderer::PlanetRenderer(Renderer& renderer) : renderer(renderer)
{
    Mesh mesh;

	int segments = 32;
	float radius = 50.0f;

	mesh.vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	for (int i = 0; i <= segments; i++) {
		float angle = i * 2.0f * std::numbers::pi / segments;
		float x = cos(angle) * radius;
		float y = sin(angle) * radius;

		mesh.vertices.push_back({x, y, 0.0f, 0.0f, 0.0f, 1.0, 1.0f, 1.0f, 1.0f});
	}
	for (int i = 1; i <= segments; i++) {
		mesh.indices.push_back(0);
		mesh.indices.push_back(i);
		mesh.indices.push_back(i + 1);
	}

    instances.resize(16);
	for(int i = 0; i < 16; i++) {
		instances[i].model = glm::translate(glm::mat4(1.0f), glm::vec3(i*100.0f, 0, 0));
		instances[i].color = glm::vec4(randomNiceColor(), 1.0f);
	}

	vkCircleMesh.updateBuffers(mesh.vertices, mesh.indices);
}

void PlanetRenderer::submit(RenderQueue& queue)
{ 
    queue.batchQueue.push_back(
        {
            &vkCircleMesh,
            renderer.vkPlanetRenderSystem.getDefaultMaterial(),
			instances
        }
    );
}