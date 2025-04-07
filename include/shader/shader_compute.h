#ifndef __SHADER_COMPUTE_H__
#define __SHADER_COMPUTE_H__
#include "resource/resource_base.h"
#include "shader/shader.h"
#include <string>

template<>
struct get_res_type<class ShaderCompute> {
    constexpr static const char* VALUE = "shader_compute";
};

class ShaderCompute : public Shader, public ResourceBase {
public:
    ShaderCompute(const std::string& p_path);
    ShaderCompute(const ShaderCompute&) = delete;
    virtual ~ShaderCompute();

    virtual void load() override;

    void compute(unsigned int p_num_groups_x,
                 unsigned int p_num_groups_y,
                 unsigned int p_num_groups_z) const;
    void use() const;
    void unuse() const;

    void set_shader_storage_barrier();
    void set_buffer_update_barrier();
    void set_va_barrier();
    void set_all_barrier();
private:
    std::string path;
    void setup(const std::string& p_path);
};
#endif
