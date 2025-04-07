#ifndef __SHADER_PIPELINE_H__
#define __SHADER_PIPELINE_H__
#include "resource/resource_base.h"
#include "shader/shader.h"

template<>
struct get_res_type<class ShaderPipeline> {
    constexpr static const char* VALUE = "shader_pipeline";
};

class ShaderPipeline : public Shader, public ResourceBase {
public:
    ShaderPipeline(const std::string& p_vertex_shader_path,
                   const std::string& p_fragment_shader_path,
                   const std::string& p_geometry_shader_path = "");

    virtual void load() override;

    virtual ~ShaderPipeline() override;

private:
    std::string vertex_shader_path;
    std::string fragment_shader_path;
    std::string geometry_shader_path;
    // Setup the shader
    void setup();
};

#endif
