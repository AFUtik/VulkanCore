#pragma once

#include <string>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

enum TextureChannels : uint32_t {
    RGBA = 4,
    RGB = 3,
    Grayscale = 1,
    GrayscaleAlpha = 2
};

class Texture2D {
public:
    std::string path;
    
    Texture2D(const uint8_t* c_ptr, int width, int height, int channels);

    Texture2D(std::unique_ptr<uint8_t[]> ptr, int width, int height, int channels);

    Texture2D(int width, int height, int channels);

    Texture2D(std::string path, TextureChannels channels = TextureChannels::RGBA);

    Texture2D() {};
    
    const uint8_t* raw() {return pixels.get();};
    
    int width = 0;
    int height = 0;
    int channels = 0;
protected:
    std::unique_ptr<uint8_t[]> pixels;

    inline uint8_t* ptr(uint32_t x, uint32_t y)
    {
        return pixels.get() + (y * width + x) * channels;
    }
    
    friend class TextureUtils;
};

class TextureUtils {
public:
    static void transfer(Texture2D* dst, Texture2D* src, uint32_t offsetX, uint32_t offsetY);
    static void save(Texture2D* texture, const std::string& path);
};