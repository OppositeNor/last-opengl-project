#include "material/material_phong.h"
#include "resource/global_resource.h"
#include "shader/shader_pipeline.h"
#include "texture/texture_2d_rgb.h"

MaterialPhong::MaterialPhong(std::vector<std::shared_ptr<Texture2D>> p_diffuse,
                  std::vector<std::shared_ptr<Texture2D>> p_normal,
                  std::vector<std::shared_ptr<Texture2D>> p_specular)
        : diffuse(p_diffuse), normal(p_normal), specular(p_specular) {
    shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_material_phong"));
    flux_shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_flux_phong"));
    depth_shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_depth_phong"));
}

void set_texture(const Shader* shader, const std::string& name,
                 const std::vector<std::shared_ptr<Texture2D>>& textures,
                 const Texture2D* default_texture, bool p_required = false) {
    shader->set_uniform(name + "_count", static_cast<int>(textures.size()));
    for (std::size_t i = 0; i < textures.size(); ++i) {
        shader->set_uniform(name + "[" + std::to_string(i) + "]", textures[i].get(), p_required);
    }
    for (std::size_t i = textures.size(); i < Material::MAX_TEXTURE_COUNT; ++i) {
        shader->set_uniform(name + "[" + std::to_string(i) + "]", default_texture, p_required);
    }
}

void MaterialPhong::set_uniform(bool p_required) const {
    auto default_texture = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d"));
    set_texture(shader.get(), "diffuse_map", diffuse, default_texture.get(), p_required);
    set_texture(shader.get(), "normal_map", normal, default_texture.get(), p_required);
    set_texture(shader.get(), "specular_map", specular, default_texture.get(), p_required);
}

void MaterialPhong::set_uniform(const char* p_param_name, bool p_required) const {
    std::string param_name(p_param_name);
    auto default_texture = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d"));
    set_texture(shader.get(), param_name + ".diffuse_map", diffuse, default_texture.get(), p_required);
    set_texture(shader.get(), param_name + ".normal_map", normal, default_texture.get(), p_required);
    set_texture(shader.get(), param_name + ".specular_map", specular, default_texture.get(), p_required);
}

void MaterialPhong::set_flux_uniform(bool p_required) const {
    auto default_texture = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d"));
    set_texture(flux_shader.get(), "diffuse_map", diffuse, default_texture.get(), p_required);
}

void MaterialPhong::set_flux_uniform(const char* p_param_name, bool p_required) const {
    std::string param_name(p_param_name);
    auto default_texture = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d"));
    set_texture(flux_shader.get(), param_name + ".diffuse_map", diffuse, default_texture.get(), p_required);
}

void MaterialPhong::set_depth_uniform(bool p_required) const {
    auto default_texture = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d"));
    set_texture(depth_shader.get(), "diffuse_map", diffuse, default_texture.get(), p_required);
}

void MaterialPhong::set_depth_uniform(const char* p_param_name, bool p_required) const {
    std::string param_name(p_param_name);
    auto default_texture = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d"));
    set_texture(depth_shader.get(), param_name + ".diffuse_map", diffuse, default_texture.get(), p_required);
}
