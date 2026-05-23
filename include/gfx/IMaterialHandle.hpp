#pragma once

namespace gfx 
{

struct Material;

class IMaterialHandle {
public:
    virtual ~IMaterialHandle() = default;

    virtual void update(const Material& mat) = 0;
    virtual void bind(void* commandBuffer) const = 0;
    virtual bool isValid() const = 0;
};

}