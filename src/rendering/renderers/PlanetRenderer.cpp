#include "rendering/renderers/PlanetRenderer.hpp"
#include "rendering/systems/BaseRenderSystem.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/RenderQueue.hpp"

#include "game/PCManager.hpp"

#include "Color.hpp"
#include <numbers>

#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

PlanetRenderer::PlanetRenderer(Renderer& renderer) : renderer(renderer), pcm(PCM::instance())
{
    std::vector<Vertex> vertices; std::vector<u32> indices; 

	int segments = 8;
	float radius = 50.0f;

	vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
	for (int i = 0; i <= segments; i++) {
		float angle = i * 2.0f * std::numbers::pi / segments;
		float x = cos(angle) * radius;
		float y = sin(angle) * radius;

		vertices.push_back({x, y, 0.0f, 0.0f, 0.0f, 1.0, 1.0f, 1.0f, 1.0f});
	}
	for (int i = 1; i <= segments; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	vkCircleMesh.updateBuffers(std::as_bytes(std::span(vertices)), indices);

	// TO REMOVE CODE //

	static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distX(-10000.0, 10000.0);
    std::uniform_real_distribution<double> distY(-10000.0, 10000.0);
	std::uniform_real_distribution<float> velX(-10, 10);
	std::uniform_real_distribution<float> velY(-10, 10);
	std::uniform_real_distribution<float> distRadius(0.2, 5.5);
	std::uniform_real_distribution<float> distMass(100, 15000);

	const size_t AMOUNT = 512;
	for(int i = 0; i < AMOUNT; i++)
	{
		PCM::Object p = pcm.create_object();
		pcm.template add_component<PCPosition>(p, glm::dvec2(distX(gen), distY(gen)));
		pcm.template add_component<PCAcceleration>(p, glm::dvec2(0.0));
		pcm.template add_component<PCVelocity>(p,     glm::dvec2(velX(gen), velY(gen)));
		pcm.template add_component<PCColor>(p, randomNiceColor());
		pcm.template add_component<PCProperties>(p, distRadius(gen), distMass(gen));
	}

	//auto& prop = pcm.get_component<PCProperties>(PCM::Object(0));
	//prop.radius = 100.0;
	//prop.mass = 1000000*50;

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
	int i = 0;
	for(auto [pos_c] : PCM::View<PCPosition>(&pcm))
	{
		instances[i].model[3].x = static_cast<float>(pos_c.position.x);
		instances[i].model[3].y = static_cast<float>(pos_c.position.y);
		i++;
	}
}

void PlanetRenderer::submit(RenderQueue& queue)
{ 
	calculateInstances();
    queue.batchQueue.push_back(
        {
            &vkCircleMesh,
            renderer.vkPlanetRenderSystem->getDefaultMaterial(),
			instances.data(),
			static_cast<u32>(pcm.size())
        }
    );
}