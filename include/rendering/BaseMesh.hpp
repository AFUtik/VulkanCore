#pragma once

#include "vk/Mesh.hpp"
#include <glm/glm.hpp>

struct Vertex
{
    float x, y, z;
    float u, v;
    float r, g, b, s;
};

struct InstanceData {
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct BaseMesh : public myvk::Mesh
{
    BaseMesh() 
    {
        setVertexStride(sizeof(Vertex));
    }
};