#include "material/material_phong_color.h"
#include "resource/global_resource.h"
#include "shader/shader_pipeline.h"

MaterialPhongColor::MaterialPhongColor(const glm::vec3& p_diffuse_color,
                                       const glm::vec3& p_specular_color)
            : diffuse_color(p_diffuse_color), specular_color(p_specular_color) {
    shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_material_phong_color"));
    flux_shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_flux_phong_color"));
    depth_shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_depth_phong_color"));
}


void MaterialPhongColor::set_uniform(bool p_required) const {
    shader->set_uniform("diffuse_color", diffuse_color, p_required);
    shader->set_uniform("specular_color", specular_color, p_required);
}

void MaterialPhongColor::set_uniform(const char* p_param_name, bool p_required) const {
    std::string param_name(p_param_name);
    shader->set_uniform(param_name + ".diffuse_color", diffuse_color, p_required);
    shader->set_uniform(param_name + ".specular_color", specular_color, p_required);
}

void MaterialPhongColor::set_flux_uniform(bool p_required) const {
    flux_shader->set_uniform("diffuse_color", diffuse_color, p_required);
}

void MaterialPhongColor::set_flux_uniform(const char* p_param_name, bool p_required) const {
    std::string param_name(p_param_name);
    flux_shader->set_uniform(param_name + ".diffuse_color", diffuse_color, p_required);
}

