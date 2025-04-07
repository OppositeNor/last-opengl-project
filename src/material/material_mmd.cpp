#include "material/material_mmd.h"
#include "resource/global_resource.h"
#include "shader/shader_pipeline.h"

MaterialMMD::MaterialMMD(const glm::vec4& p_diffuse_color, std::shared_ptr<Texture2D> p_diffuse,
                const glm::vec3& p_specular_color, std::shared_ptr<Texture2D> p_toon)
        : diffuse_color(p_diffuse_color), diffuse(p_diffuse), specular_color(p_specular_color), toon(p_toon) {
    shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_material_mmd"));
    flux_shader = nullptr; // TODO
    depth_shader = nullptr; // TODO
}

void MaterialMMD::set_uniform(bool p_required) const {
    shader->set_uniform("diffuse_color", diffuse_color, p_required);
    shader->set_uniform("diffuse", diffuse.get(), p_required);
    shader->set_uniform("specular_color", specular_color, p_required);
    shader->set_uniform("toon", toon.get(), p_required);
}

void MaterialMMD::set_uniform(const char* p_param_name, bool p_required) const {
    auto param_name = std::string(p_param_name);
    shader->set_uniform(param_name + ".diffuse_color", diffuse_color, p_required);
    shader->set_uniform(param_name + ".diffuse", diffuse.get(), p_required);
    shader->set_uniform(param_name + ".specular_color", specular_color, p_required);
    shader->set_uniform(param_name + ".toon", toon.get(), p_required);
}

    
void MaterialMMD::set_flux_uniform(bool p_required) const {
    flux_shader->set_uniform("diffuse_color", diffuse_color, p_required);
    flux_shader->set_uniform("diffuse", diffuse.get(), p_required);
}

void MaterialMMD::set_flux_uniform(const char* p_param_name, bool p_required) const {
    auto param_name = std::string(p_param_name);
    flux_shader->set_uniform(param_name + ".diffuse_color", diffuse_color, p_required);
    flux_shader->set_uniform(param_name + ".diffuse", diffuse.get(), p_required);
}

void MaterialMMD::set_depth_uniform(bool p_required) const {
    depth_shader->set_uniform("diffuse_color", diffuse_color, p_required);
}

void MaterialMMD::set_depth_uniform(const char* p_param_name, bool p_required) const {
    auto param_name = std::string(p_param_name);
    depth_shader->set_uniform(param_name + ".diffuse_color", diffuse_color, p_required);
    depth_shader->set_uniform(param_name + ".diffuse", diffuse.get(), p_required);
}
