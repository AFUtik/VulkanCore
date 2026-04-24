#include "Mesh.hpp"

/*
TextMesh::TextMesh(Text* text, Font* font) : Mesh(512), text(text), font(font) {
    buildVertices();
}


void TextMesh::buildVertices() {
    vertices.clear();
    indices.clear();
    meshContent.clear();
    indCount = 0;

    AtlasBitmap* atlasBitmap = font->fontData.bitmap.get();

    std::u32string_view content = text->getContent();
    uint32_t prevCodepoint = 0;
    for (auto codepoint : content)
    {
        uint32_t index = font->sample->getCharIndex(codepoint);
        GlyphInfo& glyph = font->fontData.infoGlyphs[index];
        if (prevCodepoint)
        {
            FT_Vector kerning = font->getKerning(prevCodepoint, codepoint);
            pen_x += kerning.x;
        }

        const Tile& tile = atlasBitmap->getTile(index);

        float x = static_cast<float>(pen_x + glyph.bearing_x);
        float y = static_cast<float>(pen_y - ((int)tile.height - glyph.bearing_y));

        float w = static_cast<float>(tile.width);
        float h = static_cast<float>(tile.height);

        // xyz, uv, color //
        vertices.push_back({x + w, y + h, 0.0f, tile.u2, tile.v1, 0, 0, 0, 1});
        vertices.push_back({x + w, y,     0.0f, tile.u2, tile.v2, 0, 0, 0, 1});
        vertices.push_back({x,     y,     0.0f, tile.u1, tile.v2, 0, 0, 0, 1});
        vertices.push_back({x,     y + h, 0.0f, tile.u1, tile.v1, 0, 0, 0, 1});

        indices.push_back(indCount+0);
        indices.push_back(indCount+1);
        indices.push_back(indCount+2);
        indices.push_back(indCount+2);
        indices.push_back(indCount+3);
        indices.push_back(indCount+0);
        indCount += 4;

        pen_x += glyph.advance_x;
        prevCodepoint = codepoint;
    }
    meshContent = content;
}


void TextMesh::build() {
    buildVertices(); 
}
*/

void MeshTools::createQuad(Mesh* mesh, Quad& quad)
{
    uint32_t vertOffset = mesh->vertices.size();

    mesh->vertices.push_back({quad.x2, quad.y2, 0.0f, quad.u2, quad.v1, quad.r, quad.g, quad.b, quad.a});
    mesh->vertices.push_back({quad.x2, quad.y1, 0.0f, quad.u2, quad.v2, quad.r, quad.g, quad.b, quad.a});
    mesh->vertices.push_back({quad.x1, quad.y1, 0.0f, quad.u1, quad.v2, quad.r, quad.g, quad.b, quad.a});
    mesh->vertices.push_back({quad.x1, quad.y2, 0.0f, quad.u1, quad.v1, quad.r, quad.g, quad.b, quad.a});

    mesh->indices.push_back(vertOffset + 0);
    mesh->indices.push_back(vertOffset + 1);
    mesh->indices.push_back(vertOffset + 2);
    mesh->indices.push_back(vertOffset + 2);
    mesh->indices.push_back(vertOffset + 3);
    mesh->indices.push_back(vertOffset + 0);
}
