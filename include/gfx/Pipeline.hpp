#pragma once

#include "Shader.hpp"
#include "management/Handle.hpp"

namespace gfx 
{

struct Shader;

enum class PrimitiveTopology
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
    LineListAdjacency,
    LineStripAdjacency,
    TriangleListAdjacency,
    TriangleStripAdjacency,
    PatchList
};

enum class BlendFactor
{
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
    Src1Color,
    OneMinusSrc1Color,
    Src1Alpha,
    OneMinusSrc1Alpha
};

enum class BlendOp
{
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};

enum class LogicOp
{
    Clear,
    Set,
    Copy, 
    CopyInverted,
    NoOp, 
    Invert,
    And,
    Nand,
    Or,
    Nor,
    Xor,
    Equivalent,
    AndReverse,
    AndInverted,
    OrReverse,
    OrInverted
};

enum class ColorWriteMask : uint8_t
{
    None = 0,
    R    = 1 << 0,
    G    = 1 << 1,
    B    = 1 << 2,
    A    = 1 << 3,
    RGB  = R | G | B,
    RGBA = R | G | B | A
};

enum class CompareOp
{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    Always
};

enum class CullMode
{
    None,
    Front,
    Back
};

enum class FillMode
{
    Fill,
    Line
};

enum class FrontFace
{
    Clockwise,
    CounterClockwise
};

enum class StencilOp
{
    Keep,
    Zero,
    Replace,
    IncrementClamp,
    DecrementClamp,
    Invert,
    IncrementWrap,
    DecrementWrap
};

struct BlendAttachmentState
{
    bool           enabled   = false;

    BlendFactor    srcColor  = BlendFactor::One;
    BlendFactor    dstColor  = BlendFactor::Zero;
    BlendOp        colorOp   = BlendOp::Add;

    BlendFactor    srcAlpha  = BlendFactor::One;
    BlendFactor    dstAlpha  = BlendFactor::Zero;
    BlendOp        alphaOp   = BlendOp::Add;

    ColorWriteMask writeMask = ColorWriteMask::RGBA;
};

struct BlendState
{
    static constexpr uint32_t kMaxAttachments = 8;

    uint32_t             attachmentCount              = 1;
    BlendAttachmentState attachments[kMaxAttachments] = {};

    bool     logicOpEnable    = false;
    LogicOp  logicOp          = LogicOp::Copy;

    float    blendConstants[4] = {0.f, 0.f, 0.f, 0.f};
};

struct StencilOpState
{
    StencilOp failOp      = StencilOp::Keep; 
    StencilOp passOp      = StencilOp::Keep;
    StencilOp depthFailOp = StencilOp::Keep;
    CompareOp compareOp   = CompareOp::Always;

    uint32_t  compareMask = 0xFF;
    uint32_t  writeMask   = 0xFF;
    uint32_t  reference   = 0;
};

struct DepthState
{
    bool      testEnable  = true;
    bool      writeEnable = true;
    CompareOp compareOp   = CompareOp::Less;

    bool      boundsEnable = false;
    float     minBounds    = 0.0f;
    float     maxBounds    = 1.0f;

    bool           stencilEnable = false;
    StencilOpState front         = {};
    StencilOpState back          = {};
};

struct DepthBiasState
{
    bool  enable         = false;
    float constantFactor = 0.0f;
    float clamp          = 0.0f;
    float slopeFactor    = 0.0f;
};

struct RasterState
{
    CullMode  cull      = CullMode::Back;
    FillMode  fill      = FillMode::Fill;
    FrontFace frontFace = FrontFace::CounterClockwise;

    DepthBiasState depthBias = {};

    bool  depthClampEnable   = false;
    bool  rasterizerDiscard  = false;

    float lineWidth          = 1.0f;
    bool  conservativeRaster = false;
};

enum class SampleCount : uint8_t
{
    x1  = 1,
    x2  = 2,
    x4  = 4,
    x8  = 8,
    x16 = 16
};

struct MultisampleState
{
    SampleCount sampleCount = SampleCount::x1;

    bool  sampleShadingEnable = false;
    float minSampleShading    = 1.0f; 

    uint64_t sampleMask       = ~0ULL;

    bool  alphaToCoverage     = false;
    bool  alphaToOne          = false;
};

enum class VertexInputRate
{
    PerVertex,
    PerInstance
};

enum class VertexFormat
{
    Float1,
    Float2,
    Float3,
    Float4,
    Half2,
    Half4,
    UInt1,  UInt2, UInt4,
    UNorm2,
    UNorm4,
    SNorm4,
    A2B10G10R10_UNorm
};

struct VertexAttribute
{
    uint32_t     location = 0; 
    uint32_t     binding  = 0;
    VertexFormat format   = VertexFormat::Float3;
    uint32_t     offset   = 0;
};

struct VertexBinding
{
    uint32_t        binding   = 0;
    uint32_t        stride    = 0;
    VertexInputRate inputRate = VertexInputRate::PerVertex;
};

struct VertexInputState
{
    static constexpr uint32_t kMaxBindings   = 16;
    static constexpr uint32_t kMaxAttributes = 32;

    uint32_t        bindingCount                  = 0;
    VertexBinding   bindings[kMaxBindings]        = {};

    uint32_t        attributeCount                = 0;
    VertexAttribute attributes[kMaxAttributes]    = {};
};

struct PipelineState
{
    VertexInputState  vertexInput   = {};
    BlendState        blend         = {};
    DepthState        depth         = {};
    RasterState       raster        = {};
    MultisampleState  multisample   = {};
    PrimitiveTopology topology      = PrimitiveTopology::TriangleList;
};

struct Pipeline
{
    inline void addShader(Handle<Shader>& shader)
    {
        shaders.push_back(shader);
    }

    inline void setShaderResourceSignature(Handle<ShaderResourceSignature>& signature)
    {
        resourceSignature = signature;
    }

    inline void setPipelineState(Handle<PipelineState>& state)
    {
        pipelineState = state;
    }
    
    virtual Handle<Pipeline> Build() = 0;
protected:
    Handle<ShaderResourceSignature> resourceSignature;
    Handle<PipelineState>           pipelineState;
    std::vector<Handle<Shader>>     shaders;
};

}