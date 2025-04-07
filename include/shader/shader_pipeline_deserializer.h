#ifndef __SHADER_DESERIALIZER_H__
#define __SHADER_DESERIALIZER_H__
#include "resource/deserializer.hpp"
#include "shader/shader_pipeline.h"

class ShaderPipelineDeserializer : public Deserializer {
public:
    ShaderPipelineDeserializer() = default;
    virtual ~ShaderPipelineDeserializer() = default;

    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) override;

    virtual std::string get_type() const override {
        return get_res_type<ShaderPipeline>::VALUE;
    }
};

#endif
