#include "gfx/vk/Shader.hpp"

#include <shaderc/shaderc.hpp>
#include <memory>

#include <iostream>
#include <fstream>

std::string ReadFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open shader: " + path);
    }

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

std::vector<uint32_t> CompileShader(const std::string& source, shaderc_shader_kind kind)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Опции (очень полезно)
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc::SpvCompilationResult result =
        compiler.CompileGlslToSpv(source, kind, "shader.glsl", options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << result.GetErrorMessage() << std::endl;
        return {};
    }

    return { result.cbegin(), result.cend() };
}

namespace vk {

Shader* ShaderManager::loadShader(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    const std::string& shaderName)
{
    auto shader = std::make_unique<Shader>();
    shader->vertexShaderFile   = vertexShaderPath;
    shader->fragmentShaderFile = fragmentShaderPath;
    shader->vertexSpirv   = CompileShader(ReadFile(vertexShaderPath), shaderc_vertex_shader);
    shader->fragmentSpirv = CompileShader(ReadFile(fragmentShaderPath), shaderc_fragment_shader);

    Shader* raw = shader.get();
    shaders.emplace(shaderName, std::move(shader));
    return raw;
};

Shader* ShaderManager::getShader(const std::string& shaderName)
{
    auto it = shaders.find(shaderName);
    if(it != shaders.end())
    { 
        return it->second.get();
    }
    else std::cerr << "Shader '" << shaderName << "' not found." << std::endl;
    return nullptr;
}

Shader* ShaderManager::getOrLoadShader(
        const std::string& vertexShaderPath,
        const std::string& fragmentShaderPath,
        const std::string& shaderName)
{
    Shader* s = getShader(shaderName);

    if(s!=nullptr) return s;
    else return loadShader(vertexShaderPath, fragmentShaderPath, shaderName);
}

}