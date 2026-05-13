#include "BaseRenderSystem.hpp"

namespace myvk
{

struct WireframeRenderSystem : public BaseRenderSystem
{
    WireframeRenderSystem(Renderer& renderer);
    WireframeRenderSystem(Renderer& renderer, RenderTarget& target);

    ~WireframeRenderSystem();
};

}