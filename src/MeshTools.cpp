#include "MeshTools.hpp"

template <>
void MeshTools::generate<MeshTools::Quad>(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const MeshTools::Quad& quad)
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