#include "spot_light.h"
#include "resource/global_resource.h"
#include "shader/shader_pipeline.h"
#include "utils.hpp"
#include <glm/ext/matrix_transform.hpp>

SpotLight::SpotLight(const glm::vec3& p_color, float p_attenuation, float p_shadow_range, unsigned int p_shadow_resolution,
                     const glm::mat4& p_proj_mat, float p_shadow_map_bias, unsigned int p_rsm_resolution, unsigned int p_lpv_resolution,
                     const glm::vec3& p_lpv_range, GIType p_gi_type)
        : color(p_color), attenuation(p_attenuation), shadow_range(p_shadow_range), shadow_map_resolution(p_shadow_resolution),
        coordinate_map(nullptr), normal_map(nullptr), flux_map(nullptr), shadow_map_bias(p_shadow_map_bias), gi_type(p_gi_type), 
        rsm_resolution(p_rsm_resolution), lpv_resolution(p_lpv_resolution), lpv_range(p_lpv_range) {
    TextureConfigurations config;
    config.generate_mipmap = false;
    config.scale_nearest_mag = true;
    config.scale_nearest_min = true;
    config.wrap_mode_s = TextureConfigurations::WrapMode::CLAMP_TO_EDGE;
    config.wrap_mode_t = TextureConfigurations::WrapMode::CLAMP_TO_EDGE;
    proj_mat = p_proj_mat;
    shadow_map = std::make_unique<FBDepth>(p_shadow_resolution, p_shadow_resolution, config,
                                           REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_spot_light_shadowmap")));
    switch (gi_type) {
        case GIType::LPV:
            sg_lpv_sh = std::make_unique<SGLPVSH>(p_lpv_resolution, this, -lpv_range, lpv_range);
            [[fallthrough]];
        case GIType::RSM:
            coordinate_map = std::make_unique<FBFRGB>(p_rsm_resolution, p_rsm_resolution, glm::vec3(0), glm::vec3(0), config,
                                                      REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_world_coordinate")));
            normal_map = std::make_unique<FBRegular>(p_rsm_resolution, p_rsm_resolution, glm::vec4(0,0,0,1), glm::vec4(0,0,0,1), config,
                                                     REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_normal")));
            flux_map = std::make_unique<FBFRGB>(p_rsm_resolution, p_rsm_resolution, glm::vec3(0), glm::vec3(0), config, nullptr);
            break;
        default:
            break;
    }

}

SpotLight::~SpotLight() {
}

