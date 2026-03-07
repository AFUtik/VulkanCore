#include "Texture.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <stb_image.h>
#include "stb_image_write.h"

#include <iostream>

Texture2D::Texture2D(std::string path, TextureChannels channels_) : path(path), channels(channels_) {
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

Texture2D::Texture2D(std::unique_ptr<uint8_t[]> ptr, int width, int height, int channels) :
    pixels(std::move(ptr)), width(width), height(height), channels(channels)
{
    
}

// Copy constructor //
Texture2D::Texture2D(const uint8_t* ptr, int width, int height, int channels) :
    width(width), height(height), channels(channels)
{
    size_t copy_size = width*height*channels;
    pixels = std::make_unique<uint8_t[]>(copy_size);
    memcpy(pixels.get(), ptr, copy_size);
}

Texture2D::Texture2D(int width, int height, int channels) :
    width(width), height(height), channels(channels)
{
    pixels = std::make_unique<uint8_t[]>(width*height*channels);
}

void TextureUtils::transfer(Texture2D *dst, Texture2D *src, uint32_t offsetX, uint32_t offsetY) {
    if(dst->channels == src->channels) {
        if(!src || src->height > dst->height || src->width > dst->width || src->channels != dst->channels) {
            std::cerr << "Failed to paste an image: src is bigger or channels are different" << std::endl;
            return;
        }
        if(offsetX + src->width > dst->width || offsetY + src->height > dst->height) {
            std::cerr << "Failed to paste an image: src is out of destination image" << std::endl;
            return;
        }

        const uint32_t bytesPerPixel = dst->channels;
        for (uint32_t y = 0; y < src->height; y++)
        {
            uint8_t* dstRow = dst->pixels.get() +
                ((offsetY + y) * dst->width + offsetX) * bytesPerPixel;
            uint8_t* srcRow = src->pixels.get() +
                (y * src->width) * bytesPerPixel;
            memcpy(dstRow, srcRow, src->width * bytesPerPixel);
        }
    } else {
        std::cout << "Failed to transfer texture: Different channels" << std::endl;
    }
}

void TextureUtils::save(Texture2D* texture, const std::string& path) {
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