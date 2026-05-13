#include "rendering/renderers/PlanetRenderer.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/RenderQueue.hpp"

#include "game/PCManager.hpp"

#include "model/Mesh.hpp"

#include "Color.hpp"
#include <numbers>

#include <glm/ext.hpp>

PlanetRenderer::PlanetRenderer(Renderer& renderer) : renderer(renderer), pcm(PCM::instance())
{
    Mesh mesh;

	int segments = 8;
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

	vkCircleMesh.updateBuffers(mesh.vertices, mesh.indices);

	// TO REMOVE CODE //

	static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distX(-10000.0, 10000.0);
    std::uniform_real_distribution<double> distY(-10000.0, 10000.0);
	std::uniform_real_distribution<float> distRadius(0.2, 1.0);

	const size_t AMOUNT = 256;
	for(int i = 0; i < AMOUNT; i++)
	{
		PCM::Object p = pcm.create_object();
		pcm.template add_component<PCPosition>(p, glm::dvec2(distX(gen), distY(gen)));
		pcm.template add_component<PCColor>(p, randomNiceColor());
		pcm.template add_component<PCProperties>(p, distRadius(gen), 0.0);
	}
	
	instances.reserve(AMOUNT);

	if(pcm.size() > instances.size()) instances.resize(pcm.size());
	PCM::View<
		PCPosition,
		PCColor, 
		PCProperties
	> view(&pcm);
	int i = 0;
	for(auto [pos_c, color_c, prop_c] : view)
	{
		instances[i].model = glm::translate(glm::mat4(1.0f), glm::vec3(pos_c.position, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(prop_c.radius));
		instances[i].color = glm::vec4(color_c.color, 1.0f);
		i++;
	}
}

void PlanetRenderer::calculateInstances()
{ 	
	
}

void PlanetRenderer::submit(RenderQueue& queue)
{ 
    queue.batchQueue.push_back(
        {
            &vkCircleMesh,
            renderer.vkPlanetRenderSystem.getDefaultMaterial(),
			instances.data(),
			static_cast<u32>(pcm.size())
        }
    );
}