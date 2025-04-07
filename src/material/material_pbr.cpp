#include "material/material_pbr.h"
#include "resource/global_resource.h"
#include "shader/shader_pipeline.h"
#include "texture/texture_2d_rgb.h"

MaterialPBR::MaterialPBR(std::vector<std::shared_ptr<Texture2D>> p_albedo_map,
                         std::vector<std::shared_ptr<Texture2D>> p_normal_map,
                         std::vector<std::shared_ptr<Texture2D>> p_metalic_map,
                         std::vector<std::shared_ptr<Texture2D>> p_roughness_map,
                         std::vector<std::shared_ptr<Texture2D>> p_ao_map)
    : albedo_map(p_albedo_map), normal_map(p_normal_map),
    metalic_map(p_metalic_map), roughness_map(p_roughness_map), ao_map(p_ao_map) {
    shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_material_pbr"));
    flux_shader = GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_flux_pbr");
    depth_shader = GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_depth_pbr");
}

template<typename TextureType>
void set_texture(const Shader* shader, const std::string& name,
                 const std::vector<std::shared_ptr<TextureType>>& textures,
                 const Texture2D* default_texture, bool p_required = false) {
    shader->set_uniform(name + "_count", static_cast<int>(textures.size()));
    for (std::size_t i = 0; i < textures.size(); ++i) {
        shader->set_uniform(name + "[" + std::to_string(i) + "]", textures[i].get(), p_required);
    }
    for (std::size_t i = textures.size(); i < Material::MAX_TEXTURE_COUNT; ++i) {
        shader->set_uniform(name + "[" + std::to_string(i) + "]", default_texture, p_required);
    }
}

void MaterialPBR::set_uniform(bool p_required) const {
    auto default_texture = GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d").get();
    set_texture(shader.get(), "albedo_map", albedo_map, default_texture, p_required);
    set_texture(shader.get(), "normal_map", normal_map, default_texture, p_required);
    set_texture(shader.get(), "metalic_map", metalic_map, default_texture, p_required);
    set_texture(shader.get(), "roughness_map", roughness_map, default_texture, p_required);
    set_texture(shader.get(), "ao_map", ao_map, default_texture, p_required);
}

void MaterialPBR::set_uniform(const char* p_param_name, bool p_required) const {
    auto default_texture = GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d").get();
    std::string param_name(p_param_name);
    set_texture(shader.get(), param_name + ".albedo_map", albedo_map, default_texture, p_required);
    set_texture(shader.get(), param_name + ".normal_map", normal_map, default_texture, p_required);
    set_texture(shader.get(), param_name + ".metalic_map", metalic_map, default_texture, p_required);
    set_texture(shader.get(), param_name + ".roughness_map", roughness_map, default_texture, p_required);
    set_texture(shader.get(), param_name + ".ao_map", ao_map, default_texture, p_required);
}

void MaterialPBR::set_flux_uniform(bool p_required) const {
    auto default_texture = GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d").get();
    set_texture(flux_shader.get(), "albedo_map", albedo_map, default_texture, p_required);
}

void MaterialPBR::set_flux_uniform(const char* p_param_name, bool p_required) const {
    auto default_texture = GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d").get();
    std::string param_name(p_param_name);
    set_texture(flux_shader.get(), param_name + ".albedo_map", albedo_map, default_texture, p_required);
}

void MaterialPBR::set_depth_uniform(bool p_required) const {
    auto default_texture = GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d").get();
    set_texture(depth_shader.get(), "albedo_map", albedo_map, default_texture, p_required);
}

void MaterialPBR::set_depth_uniform(const char* p_param_name, bool p_required) const {
    auto default_texture = GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d").get();
    std::string param_name(p_param_name);
    set_texture(depth_shader.get(), param_name + ".albedo_map", albedo_map, default_texture, p_required);
}
