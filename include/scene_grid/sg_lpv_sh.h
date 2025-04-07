#ifndef __SG_LPV_SH_H__
#define __SG_LPV_SH_H__
#include "scene_grid.hpp"
#include "shader/shader_compute.h"
#include "texture/texture_3d_frgba.h"
#include <memory>

class SGLPVSH : public SceneGrid {
public:
    SGLPVSH(unsigned int p_lpv_resolution,
            class SpotLight* p_spot_light,
            const glm::vec3& p_left_bottom_back,
            const glm::vec3& p_right_top_front);

    virtual ~SGLPVSH() override {}
    
    virtual void dispatch_compute() const override;

    const glm::vec3& get_left_bottom_back() const { return left_bottom_back; }
    const glm::vec3& get_right_top_front() const { return right_top_front; }
    void set_left_bottom_back(const glm::vec3& p_left_bottom_back) { left_bottom_back = p_left_bottom_back; }
    void set_right_top_front(const glm::vec3& p_right_top_front) { right_top_front = p_right_top_front; }

private:
    std::shared_ptr<Texture3DFRGBA> lpv_sh_r;
    std::shared_ptr<Texture3DFRGBA> lpv_sh_g;
    std::shared_ptr<Texture3DFRGBA> lpv_sh_b;
    SpotLight* spot_light = nullptr;
    std::shared_ptr<ShaderCompute> inject_compute_shader;
    std::shared_ptr<ShaderCompute> propagate_compute_shader;
    glm::vec3 left_bottom_back;
    glm::vec3 right_top_front;
};

#endif
