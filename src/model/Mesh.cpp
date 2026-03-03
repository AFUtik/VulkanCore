#include "Mesh.hpp"
#include "../Fonts.hpp"
#include "freetype/ftimage.h"

#include <iostream>
#include <string_view>

namespace myvk {


TextMesh::TextMesh(Text* text) : MeshInstance(512), text(text) {
    buildVertices();
}

void TextMesh::buildVertices() {
    vertices.clear();
    indices.clear();
    meshContent.clear();
    indCount = 0;

    Font* font = text->getFont();
    AtlasBitmap& atlasBitmap = font->fontData.bitmap;

    pen_y = font->fontData.baseline_y;

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

        const Tile& tile = atlasBitmap.getTile(index);

        float x = static_cast<float>(pen_x + glyph.bearing_x);
        float y = static_cast<float>(pen_y - (tile.height - glyph.bearing_y));

        float w = static_cast<float>(tile.width);
        float h = static_cast<float>(tile.height);

        // xyz, uv, color //
        vertices.push_back({x + w, y + h, 0.0f, tile.u2, tile.v1, 1, 1, 1, 1});
        vertices.push_back({x + w, y,     0.0f, tile.u2, tile.v2, 1, 1, 1, 1});
        vertices.push_back({x,     y,     0.0f, tile.u1, tile.v2, 1, 1, 1, 1});
        vertices.push_back({x,     y + h, 0.0f, tile.u1, tile.v1, 1, 1, 1, 1});

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

}