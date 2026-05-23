#pragma once

#include <string>
#include <vector>
#include <variant>
#include <memory>

#include <glm/glm.hpp>

struct Texture;

namespace gfx
{

using ParamValue = std::variant<
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    std::shared_ptr<Texture>
>;

struct ParamDecl {
    std::string  name;
    ParamValue   defaultValue; 
};

struct MaterialTemplate {
    const std::string& getName() const { return name_; }

    const std::vector<ParamDecl>& getParams() const { return params_; }

    static std::shared_ptr<MaterialTemplate> PBR();
    static std::shared_ptr<MaterialTemplate> Unlit();
private:
    std::string            name_;
    std::vector<ParamDecl> params_;
};

}