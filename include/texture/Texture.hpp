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

enum class TextureFilter {
	Linear,
	Nearest,
	LinearMipmap,
	NearestMipmap
};

// Contains data about image not vulkan image //
class Texture {
public:
    Texture(std::string path, TextureChannels channels = TextureChannels::RGBA);
    
    Texture(const uint8_t* c_ptr, int width, int height, int channels);

    Texture(std::unique_ptr<uint8_t[]> ptr, int width, int height, int channels);

    Texture(int width, int height, int channels);

    Texture() = default;
    
    uint8_t* raw() 
    {
        return pixels.get();
    };
    
    inline uint8_t* ptr(uint32_t x, uint32_t y)
    {
        return pixels.get() + (y * width + x) * channels;
    }
    
    int width    = 0;
    int height   = 0;
    int channels = 0;
protected:
    std::unique_ptr<uint8_t[]> pixels;

    friend class TextureUtils;
};

class TextureUtils {
public:
    static void transfer(
        Texture* dst, 
        Texture* src, 
        uint32_t offsetX, 
        uint32_t offsetY);

    static void transferRegion(Texture* dst, Texture* src,
                    uint32_t dx, uint32_t dy,
                    uint32_t sx, uint32_t sy,
                    uint32_t w, uint32_t h); 

    static void extrudePadding(Texture* atlas,
                    uint32_t x, uint32_t y,
                    uint32_t w, uint32_t h,
                    uint32_t padding);

    static void save(Texture* texture, const std::string& path);
};