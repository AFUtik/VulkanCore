#pragma once

#include <stdexcept>
#include <string>
#include <array>
#include <algorithm>
#include <random>

#include <glm/glm.hpp>

inline std::mt19937 rng(std::random_device{}());
inline std::uniform_real_distribution<float> dist(0.0f, 1.0f);

struct Color {
    constexpr Color() = default;
    constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static constexpr inline Color Red()   { return {1, 0, 0, 1}; }
    static constexpr inline Color Green() { return {0, 1, 0, 1}; }
    static constexpr inline Color Blue()  { return {0, 0, 1, 1}; }
    static constexpr inline Color White() { return {1, 1, 1, 1}; }
    static constexpr inline Color Black() { return {0, 0, 0, 1}; }
    static constexpr inline Color Transparent() { return {0, 0, 0, 0}; }

    /*
    static inline Color random() {
        
    }
    */
    
    static inline Color lerp(const Color& a, const Color& b, float t) {
        return {
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
        };
    }

    static inline Color ColorFromHex(const std::string& hex) 
    {
        if (hex.empty() || hex[0] != '#') throw std::runtime_error("Invalid color format");

        std::string h = hex.substr(1);

        uint8_t r, g, b, a = 255;

        if (h.size() == 6) {
            r = hexByte(h, 0);
            g = hexByte(h, 2);
            b = hexByte(h, 4);
        }
        else if (h.size() == 8) {
            r = hexByte(h, 0);
            g = hexByte(h, 2);
            b = hexByte(h, 4);
            a = hexByte(h, 6);
        }
        else throw std::runtime_error("Invalid hex length");
        return {
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            a / 255.0f
        };
    }

    Color operator+(const Color& c) const {
        return {r + c.r, g + c.g, b + c.b, a + c.a};
    }

    Color operator*(float f) const {
        return {r * f, g * f, b * f, a * f};
    }

    void clamp() {
        r = std::clamp(r, 0.0f, 1.0f);
        g = std::clamp(g, 0.0f, 1.0f);
        b = std::clamp(b, 0.0f, 1.0f);
        a = std::clamp(a, 0.0f, 1.0f);
    }

    std::array<float, 4> array() const {
        return {r, g, b, a};
    }

    glm::vec4 vec4() const {
        return {r, g, b, a};
    }

    glm::vec3 vec3() const {
        return {r, g, b};
    }
    
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
private:
    static inline uint8_t hexByte(const std::string& str, int i) {
        auto hex = str.substr(i, 2);
        return static_cast<uint8_t>(std::stoul(hex, nullptr, 16));
    }
};