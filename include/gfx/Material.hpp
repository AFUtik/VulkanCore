#pragma once

#include "gfx/MaterialTemplate.hpp"

namespace gfx
{

class Material {
public:
    explicit Material(std::shared_ptr<MaterialTemplate> tmpl)
        : template_(std::move(tmpl)) {}

    const MaterialTemplate* getTemplate() const { return template_.get(); }

    void setParam(std::string_view name, ParamValue value) {
        params_[std::string(name)] = std::move(value);
        dirty_ = true;
    }

    const ParamValue* getParam(std::string_view name) const {
        auto it = params_.find(std::string(name));
        if (it != params_.end()) return &it->second;

        for (auto& decl : template_->getParams())
            if (decl.name == name) return &decl.defaultValue;

        return nullptr;
    }

    void setTexture(std::string_view name, std::shared_ptr<Texture> tex) {
        setParam(name, std::move(tex));
    }
    void setFloat(std::string_view name, float v) { setParam(name, v); }
    void setVec3 (std::string_view name, glm::vec3 v) { setParam(name, v); }

    bool isDirty() const  { return dirty_; }
    void markClean()      { dirty_ = false; }

private:
    std::shared_ptr<MaterialTemplate>              template_;
    std::unordered_map<std::string, ParamValue>    params_;
    bool                                           dirty_ = true;
};

}