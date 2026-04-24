#pragma once

#include "../texture/Texture.hpp"

#include <unordered_map>

struct TileRegion
{
    float u1, v1, u2, v2;
};

struct Tile
{
    uint32_t tilesetId;
    uint32_t tileId;
};

struct Tileset
{
    std::unordered_map<std::string, uint32_t> tileMap;
    Texture texture;
    int tileW = 0;
    int tileH = 0;
    int tilesW = 0;
    int tilesH = 0;
    int padding = 1;

    Tileset(
        const std::string& path,
        int tileW,
        int tileH,
        int padding = 1
    );

    inline TileRegion region(uint32_t tileId) const
    {
        TileRegion uv;
        uv.u1 = (tileW+padding*2)*(tileId%tilesW);
        uv.v1 = (tileH+padding*2)*(tileId/tilesW);
        uv.u2 = uv.u1 + tileW;
        uv.v2=  uv.v1 + tileH;
        return uv;
    }
private:
    void addPadding(Texture& dst, Texture& src);
};