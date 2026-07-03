#include "gfx/Backend.hpp"
#include "gfx/BindGroup.hpp"
#include "gfx/BindGroupLayout.hpp"
#include "gfx/ColorTarget.hpp"
#include "gfx/Framebuffer.hpp"
#include "gfx/Handle.hpp"
#include "gfx/IImage.hpp"
#include "gfx/IMesh.hpp"
#include "gfx/IShader.hpp"
#include "gfx/PipelineLayout.hpp"
#include "gfx/PipelineState.hpp"
#include "gfx/RenderPass.hpp"
#include "gfx/RenderPipeline.hpp"
#include "gfx/Vertex.hpp"

#include "Window.hpp"
#include "Events.hpp"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparties/stb_image.h" 

// Vertex Shader
const char* vertexShaderSrc = R"(
#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aColor;

out vec3 f_color;
out vec2 tex_coord;

void main() {
    f_color   = aColor;
    tex_coord = aUv;

    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment Shader
const char* fragmentShaderSrc = R"(
#version 430 core

in vec3 f_color;
in vec2 tex_coord;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D uTexture;

void main() {
    vec4 color = texture(uTexture, tex_coord) * vec4(f_color, 1.0f);
    FragColor = color;
}
)";

struct Texture
{
    std::unique_ptr<uint8_t> pixels;
    int32_t width;
    int32_t height;
    int32_t channels;
};

const std::string absPath = "/home/afutik/code/cplusplus/GFX/";

int main() {
    Window windowInstance;
    windowInstance.init(800, 600, "GameEngine 0.0.1");

    Events::init(&windowInstance);

    auto device = gfx::createOpenGLBackend(&windowInstance);

    // Screen Framebuffer
    gfx::Framebuffer& fbo = device->getScreenFramebuffer();
    fbo.resize(800, 600);

    // Shaders
    gfx::Handle<gfx::Shader> vshader = device->createShader(
        gfx::ShaderDesc{
            .name  = "VertexShader",
            .spirv = {}, 
            .glsl  = vertexShaderSrc,
            .stage = gfx::ShaderStage::Vertex
        }
    );
    gfx::Handle<gfx::Shader> fshader = device->createShader(
        gfx::ShaderDesc{
            .name  = "FragmentShader",
            .spirv = {}, 
            .glsl  = fragmentShaderSrc,
            .stage = gfx::ShaderStage::Fragment
        }
    );

    // BindGroupLayout 
    gfx::Handle<gfx::BindGroupLayout> bindGroupLayout = device->createBindGroupLayout(
        gfx::BindGroupLayoutDesc{}
                .add(
                    gfx::BindGroupLayoutEntry{
                        .type = gfx::TextureTypeStruct{.sample_type = gfx::TextureSampleType::Float},
                        .binding = 0,
                        .visibility = (uint32_t)gfx::ShaderStage::Fragment}
                )
    );

    // BindGroup
        // Image loading
    stbi_set_flip_vertically_on_load(true);
    Texture texture;
    texture.pixels.reset(
        stbi_load(
            (absPath+"kharkiv.png").c_str(),
            &texture.width,
            &texture.height,
            &texture.channels,
            4
        )
    );

    gfx::Handle<gfx::Image> image = device->createImage(
        gfx::ImageDesc{
            .width    = (uint32_t)texture.width,
            .height   = (uint32_t)texture.height,
            .channels = 4,
            .filter   = gfx::ImageFilter::NEAREST
        }
    );
    image->write(texture.pixels.get());

        // Setting Bind Group
    gfx::BindGroup bindGroup{
        .layout  = bindGroupLayout,
        .entries = {gfx::BindGroupEntry{
            .binding  = 0,
            .resource = image.Cast<gfx::unknown_type>()
        }},
    };

    // Pipeline Layout
    gfx::Handle<gfx::PipelineLayout> pipelineLayout = device->createPipelineLayout(
        gfx::PipelineLayoutDesc{.layouts = {bindGroupLayout}}
    );

    // Pipeline State
    gfx::Handle<gfx::PipelineState> pipelineState = device->createPipelineState();
    pipelineState->blend.attachments[0] = {
        .enabled  = true,

        .srcColor = gfx::BlendFactor::SrcAlpha,
        .dstColor = gfx::BlendFactor::OneMinusSrcAlpha,
        .colorOp  = gfx::BlendOp::Add,

        .srcAlpha = gfx::BlendFactor::One,
        .dstAlpha = gfx::BlendFactor::OneMinusSrcAlpha,
        .alphaOp  = gfx::BlendOp::Add,
    };

    // Render Pipeline
    gfx::RenderPipelineDesc pipelineDesc{};
    pipelineDesc.pipelineLayout = pipelineLayout;
    pipelineDesc.pipelineState  = pipelineState;
    pipelineDesc.vertexState    = gfx::VertexState{
        .module = vshader,
        .layout = gfx::Vertex::getLayout()
    };
    pipelineDesc.fragState      = gfx::FragmentState{
        .module  = fshader,
        .targets = {}
    };
    gfx::Handle<gfx::RenderPipeline> pipeline = device->createRenderPipeline(pipelineDesc);

    // RenderPass
    gfx::Handle<gfx::RenderPass> pass = device->createRenderPass(
        gfx::RenderPassDesc{
            .attachments = {
                gfx::ColorAttachment{
                    .type    = gfx::AttachmentType::ATTACHMENT_TYPE_COLOR,
                    .format  = gfx::ImageFormat::RGBA8,
                    .loadOp  = gfx::LoadOp::LOAD_OP,
                    .storeOp = gfx::StoreOp::STORE_OP 
                }
            },
            .clearColor  = {0.4f, 0.4f, 0.4f, 1.0f}
        }
    );

    std::vector<gfx::Vertex> vertices = {
        gfx::Vertex{{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        gfx::Vertex{{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        gfx::Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        gfx::Vertex{{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
    };
    std::vector<uint32_t> indices = {
        0, 1, 3,
        1, 2, 3
    };

    gfx::Handle<gfx::MeshDesc> desc = device->meshDescHandle(
        gfx::MeshDesc{
            .layout = gfx::Vertex::getLayout(),
            .indexStride = sizeof(uint32_t)
        }
    );

    gfx::Handle<gfx::Mesh> mesh = device->createMesh(desc);
    mesh->updateVertices(vertices.data(), vertices.size());
    mesh->updateIndexes (indices.data(), indices.size());

    while (!windowInstance.isShouldClose()) {
        if(Events::jpressed(GLFW_KEY_ESCAPE))
        {
            return 0;
        }

        pass->begin(&fbo);

        pass->setRenderPipeline(pipeline.Get());
        pass->setBindGroup(&bindGroup);

        mesh->draw();

        pass->end();

        windowInstance.swapBuffers();
        Events::pullEvents();
    }
    return 0;
}
