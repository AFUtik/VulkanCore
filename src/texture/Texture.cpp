#include "texture/Texture.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <stb_image.h>
#include "stb_image_write.h"

#include <iostream>

Texture::Texture(std::string path, TextureChannels channels_) : channels(channels_) {
    pixels.reset(stbi_load(path.c_str(), &width, &height, &channels, channels_));
    channels = channels_;
    if (!pixels) {
    #ifndef _WIN64
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current working dir: %s\n", cwd);
        } else {
            printf("getcwd() error\n");
        }
    #endif
        printf("Error loading Texture from '%s'\n", path.c_str());
        exit(1);
    }
}

Texture::Texture(std::unique_ptr<uint8_t[]> ptr, int width, int height, int channels) :
    pixels(std::move(ptr)), width(width), height(height), channels(channels)
{
    
}

// Copy constructor //
Texture::Texture(const uint8_t* ptr, int width, int height, int channels) :
    width(width), height(height), channels(channels)
{
    size_t copy_size = width*height*channels;
    pixels = std::make_unique<uint8_t[]>(copy_size);
    memcpy(pixels.get(), ptr, copy_size);
}

Texture::Texture(int width, int height, int channels) :
    width(width), height(height), channels(channels)
{
    pixels = std::make_unique<uint8_t[]>(width*height*channels);
}

void TextureUtils::transfer(Texture* dst, Texture* src, uint32_t offsetX, uint32_t offsetY)
{
    if (!dst || !src) {
        std::cerr << "Null texture pointer\n";
        return;
    }

    if (dst->channels != src->channels) {
        std::cerr << "Different channels\n";
        return;
    }

    if (offsetX + src->width > dst->width ||
        offsetY + src->height > dst->height) {
        std::cerr << "Out of bounds\n";
        return;
    }

    const uint32_t bpp = dst->channels;
    for (uint32_t y = 0; y < src->height; y++) {

        uint8_t* dstRow = dst->pixels.get() +
            ((offsetY + y) * dst->width + offsetX) * bpp;

        uint8_t* srcRow = src->pixels.get() +
            (y * src->width) * bpp;

        memcpy(dstRow, srcRow, src->width * bpp);
    }
}

void TextureUtils::extrudePadding(Texture* atlas,
                    uint32_t x, uint32_t y,
                    uint32_t w, uint32_t h,
                    uint32_t padding)
{
    const uint32_t bpp = atlas->channels;
    uint8_t* pixels = atlas->raw();
    uint32_t atlasW = atlas->width;

    for (uint32_t i = 0; i < padding; i++) {
        // Up
        memcpy(
            pixels + ((y - 1 - i) * atlasW + x) * bpp,
            pixels + (y * atlasW + x) * bpp,
            w * bpp
        );

        // Down
        memcpy(
            pixels + ((y + h + i) * atlasW + x) * bpp,
            pixels + ((y + h - 1) * atlasW + x) * bpp,
            w * bpp
        );
    }

    for (uint32_t row = 0; row < h; row++) {
        uint8_t* rowPtr = pixels + ((y + row) * atlasW) * bpp;

        for (uint32_t i = 0; i < padding; i++) {
            // Left
            memcpy(
                rowPtr + (x - 1 - i) * bpp,
                rowPtr + x * bpp,
                bpp
            );

            // Right
            memcpy(
                rowPtr + (x + w + i) * bpp,
                rowPtr + (x + w - 1) * bpp,
                bpp
            );
        }
    }

    for (uint32_t py = 0; py < padding; py++) {
        for (uint32_t px = 0; px < padding; px++) {
            // TL
            memcpy(
                pixels + ((y - 1 - py) * atlasW + (x - 1 - px)) * bpp,
                pixels + (y * atlasW + x) * bpp,
                bpp
            );

            // TR
            memcpy(
                pixels + ((y - 1 - py) * atlasW + (x + w + px)) * bpp,
                pixels + (y * atlasW + (x + w - 1)) * bpp,
                bpp
            );

            // BL
            memcpy(
                pixels + ((y + h + py) * atlasW + (x - 1 - px)) * bpp,
                pixels + ((y + h - 1) * atlasW + x) * bpp,
                bpp
            );

            // BR
            memcpy(
                pixels + ((y + h + py) * atlasW + (x + w + px)) * bpp,
                pixels + ((y + h - 1) * atlasW + (x + w - 1)) * bpp,
                bpp
            );
        }
    }
}

void TextureUtils::transferRegion(Texture* dst, Texture* src,
                    uint32_t dx, uint32_t dy,
                    uint32_t sx, uint32_t sy,
                    uint32_t w, uint32_t h)
{
    const uint32_t bpp = dst->channels;

    for (uint32_t y = 0; y < h; y++) {
        uint8_t* dstRow = dst->pixels.get() +
            ((dy + y) * dst->width + dx) * bpp;

        uint8_t* srcRow = src->pixels.get() +
            ((sy + y) * src->width + sx) * bpp;

        memcpy(dstRow, srcRow, w * bpp);
    }
}

void TextureUtils::save(Texture* texture, const std::string& path) {
    if(texture->width == 0 || texture->height == 0 || texture->channels == 0 || !texture->pixels) {
        std::cerr << " Can't save image: data or parameters is empty" << std::endl;
        return;
    } 
    stbi_write_png(
        path.c_str(),
        texture->width,
        texture->height,
        texture->channels,
        texture->pixels.get(),
        texture->width * texture->channels  // stride (bytes per row)
    );
}