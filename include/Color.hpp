#pragma once

#include <array>
#include <algorithm>
#include <random>

#include <glm/glm.hpp>

struct ColorRGBA {
    constexpr ColorRGBA() = default;
    constexpr ColorRGBA(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static constexpr inline ColorRGBA Red()   { return {1, 0, 0, 1}; }
    static constexpr inline ColorRGBA Green() { return {0, 1, 0, 1}; }
    static constexpr inline ColorRGBA Blue()  { return {0, 0, 1, 1}; }
    static constexpr inline ColorRGBA White() { return {1, 1, 1, 1}; }
    static constexpr inline ColorRGBA Black() { return {0, 0, 0, 1}; }
    static constexpr inline ColorRGBA Transparent() { return {0, 0, 0, 0}; }

    static inline ColorRGBA lerp(const ColorRGBA& a, const ColorRGBA& b, float t) {
        return {
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
    };
}

    ColorRGBA operator+(const ColorRGBA& c) const {
        return {r + c.r, g + c.g, b + c.b, a + c.a};
    }

    ColorRGBA operator*(float f) const {
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
private:
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

using ColorRGB = glm::vec3;

static inline ColorRGB randomColor() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    return {
        dist(gen),
        dist(gen),
        dist(gen)
    };
}

static inline ColorRGB randomNiceColor() {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> hue(0.0f, 360.0f);

    float h = hue(gen);
    float s = 0.7f;
    float v = 0.9f;

    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    float r, g, b;
    if (h < 60)       { r=c; g=x; b=0; }
    else if (h < 120) { r=x; g=c; b=0; }
    else if (h < 180) { r=0; g=c; b=x; }
    else if (h < 240) { r=0; g=x; b=c; }
    else if (h < 300) { r=x; g=0; b=c; }
    else              { r=c; g=0; b=x; }

    return {r+m, g+m, b+m};
}