#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace myvk {

struct Shader
{
    void reload() {};

    inline std::vector<uint32_t>& getVertexCode() {return vertexSpirv;};
    inline std::vector<uint32_t>& getFragmentCode() {return fragmentSpirv;};
private:
    std::string vertexShaderFile;
    std::string fragmentShaderFile;
    std::vector<uint32_t> vertexSpirv;
    std::vector<uint32_t> fragmentSpirv;

    friend class ShaderManager;
};

struct ShaderManager
{
    Shader* loadShader(
        const std::string& vertexShaderPath,
        const std::string& fragmentShaderPath,
        const std::string& shaderName
    );

    Shader* getOrLoadShader(
        const std::string& vertexShaderPath,
        const std::string& fragmentShaderPath,
        const std::string& shaderName
    );

    Shader* getShader(const std::string& shaderName);
private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
};

}