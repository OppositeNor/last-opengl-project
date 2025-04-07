#include "point_light.h"
#include "system/system.h"
#include "shader/shader_pipeline.h"
#include "resource/global_resource.h"
#include <glm/gtc/matrix_transform.hpp>

PointLight::PointLight(const glm::vec3& p_color, float p_attenuation, unsigned int p_shadow_map_resolution, float p_shadow_range)
    : shadow_range(p_shadow_range), position(glm::vec3(0)), color(p_color), attenuation(p_attenuation) {
    auto proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, shadow_range);
    proj_matrices[SHADOW_MAP_RIGHT] = proj * glm::lookAt(glm::vec3(0.0f), glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0));
    proj_matrices[SHADOW_MAP_LEFT]  = proj * glm::lookAt(glm::vec3(0.0f), glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0));
    proj_matrices[SHADOW_MAP_UP]    = proj * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1));
    proj_matrices[SHADOW_MAP_DOWN]  = proj * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1));
    proj_matrices[SHADOW_MAP_FRONT] = proj * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0));
    proj_matrices[SHADOW_MAP_BACK]  = proj * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0));
    auto exe_directory = System::get_singleton()->get_exe_directory();
    depth_shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_cube_depth"));
    shadow_map = new CubeDepthFramebuffer(p_shadow_map_resolution);
}

PointLight::~PointLight() {
    delete shadow_map;
}

glm::mat4 PointLight::get_vp(int p_index) const {
    return proj_matrices[p_index] * glm::translate(glm::mat4(1.0f), -position);
}
