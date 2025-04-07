#include "scene_grid/sg_lpv_sh.h"
#include "resource/global_resource.h"
#include "spot_light.h"
#include "texture/texture_3d_frgba.h"

SGLPVSH::SGLPVSH(unsigned int p_lpv_resolution,
                 SpotLight* p_spot_light,
                 const glm::vec3& p_left_bottom_back,
                 const glm::vec3& p_right_top_front)
    : spot_light(REQUIRE_NON_NULL(p_spot_light)), left_bottom_back(p_left_bottom_back), right_top_front(p_right_top_front) {
    TextureConfigurations config;
    config.generate_mipmap = false;
    config.scale_nearest_mag = true;
    config.scale_nearest_min = true;
    config.srgb = false;
    lpv_sh_r = std::make_shared<Texture3DFRGBA>(p_lpv_resolution, p_lpv_resolution, p_lpv_resolution, config);
    lpv_sh_g = std::make_shared<Texture3DFRGBA>(p_lpv_resolution, p_lpv_resolution, p_lpv_resolution, config);
    lpv_sh_b = std::make_shared<Texture3DFRGBA>(p_lpv_resolution, p_lpv_resolution, p_lpv_resolution, config);
    inject_compute_shader = GlobalResource::get_singleton()->get_resource<ShaderCompute>("shader_lpv_inject_compute");
    propagate_compute_shader = GlobalResource::get_singleton()->get_resource<ShaderCompute>("shader_lpv_propagate_compute");
}

void SGLPVSH::dispatch_compute() const {
    // inject
    unsigned int rsm_resolution = spot_light->get_rsm_resolution();
    lpv_sh_r->bind_compute(0, 0);
    lpv_sh_g->bind_compute(1, 0);
    lpv_sh_b->bind_compute(2, 0);
    inject_compute_shader->set_uniform("lpv_bounds_lbb", spot_light->get_position() + left_bottom_back);
    inject_compute_shader->set_uniform("lpv_bounds_rtf", spot_light->get_position() + right_top_front);
    int width = lpv_sh_r->get_width();
    inject_compute_shader->set_uniform("lpv_dimensions", glm::vec3(width, width, width));
    inject_compute_shader->set_uniform("spot_light", spot_light);
    inject_compute_shader->compute(rsm_resolution / 8 + 1, rsm_resolution / 8 + 1, 1);
    inject_compute_shader->set_all_barrier(); // TODO

    // TODO: propagate
    // propagate_compute_shader->compute(width / 4 + 1, width / 4 + 1, width / 4 + 1);
    // propagate_compute_shader->set_all_barrier(); // TODO
}
