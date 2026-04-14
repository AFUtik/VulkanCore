#pragma once

#include <array>
#include <algorithm>

#include <glm/glm.hpp>

struct Color {
    constexpr Color() = default;
    constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static constexpr inline Color Red()   { return {1, 0, 0, 1}; }
    static constexpr inline Color Green() { return {0, 1, 0, 1}; }
    static constexpr inline Color Blue()  { return {0, 0, 1, 1}; }
    static constexpr inline Color White() { return {1, 1, 1, 1}; }
    static constexpr inline Color Black() { return {0, 0, 0, 1}; }
    static constexpr inline Color Transparent() { return {0, 0, 0, 0}; }

    static inline Color lerp(const Color& a, const Color& b, float t) {
        return {
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
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
private:
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};