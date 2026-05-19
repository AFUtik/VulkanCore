#pragma once

#include "rendering/BaseMesh.hpp"
#include <vector>

class MeshTools {
public:
    struct Quad 
    {
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
        float u1 = 0.0f, v1 = 0.0f, u2 = 0.0f, v2 = 0.0f;
        float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    };

    struct Circle
    {
        uint32_t segments;
        float radius;
    };

    struct Sphere
    {
        uint32_t segments; 
        float radius;
    };

    template <typename T>
    static void generate(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const T& obj);
};
