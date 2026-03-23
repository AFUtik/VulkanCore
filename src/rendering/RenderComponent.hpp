#pragma once

#include "RenderService.hpp"

class IRenderComponent {
public:
    virtual void render() {};
};

template<typename Object>
class RenderComponent : public IRenderComponent 
{
protected:
    Object* obj;

    HandleContainer handleContainer;
    RenderService*  service = nullptr;
public:
    RenderComponent(RenderService* service);
};