#pragma once

#include <string>
#include <vector>

class Text;

namespace myvk 
{

class Mesh {
public:
    struct Vertex {
        float x, y, z;
        float u, v;
        float r, g, b, s;
    };

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
    uint32_t indCount = 0;
    
	Mesh(uint32_t reservedVertices, uint32_t reservedIndices = 0) {
        vertices.reserve(reservedVertices);
        if(reservedIndices) indices.reserve(reservedIndices);
    };
    Mesh() {};
	~Mesh() {};

    Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
};

class TextMesh : public Mesh {
    Text* text = nullptr;
    std::u32string meshContent;

    int pen_x = 0;
    int pen_y = 0;

    void buildVertices();
public:
    TextMesh(Text* text);

    void build();
};

class MeshTools {
public:
    struct Quad {
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
        float u1 = 0.0f, v1 = 0.0f, u2 = 0.0f, v2 = 0.0f;
        float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    };

    static void createQuad(Mesh& instance, Quad& quad);
};

}