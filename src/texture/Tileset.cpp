#include "texture/Tileset.hpp"
#include "texture/Texture.hpp"

Tileset::Tileset(
        const std::string& path,
        int tileW,
        int tileH,
        int padding) : tileW(tileW), tileH(tileH), padding(padding)
{   
    Texture source(path, TextureChannels::RGBA);
    tilesW = source.width / tileW;
    tilesH = source.height / tileH;
    
    texture = Texture(tilesW * (tileW + padding*2), tilesH * (tileH + padding*2), TextureChannels::RGBA);

    addPadding(texture, source);
}

void Tileset::addPadding(Texture& dst, Texture& src) 
{
    for(int i = 0; i < tilesW; i++) 
    {
        int srcX = i * tileW;
        int dstX = i * (tileW + 2 * padding) + padding;

        for(int j = 0; j < tilesH; j++) 
        {
            int srcY = j * tileH;
            int dstY = j * (tileH + 2 * padding) + padding;

            TextureUtils::transferRegion(
                &dst, &src,
                dstX, dstY,
                srcX, srcY,
                tileW, tileH
            );

            TextureUtils::extrudePadding(
                &dst,
                dstX, dstY,
                tileW, tileH,
                padding
            );
        }
    }
}