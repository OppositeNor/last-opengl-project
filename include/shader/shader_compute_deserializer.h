#ifndef __SHADER_COMPUTE_DESERIALIZER_H__
#define __SHADER_COMPUTE_DESERIALIZER_H__
#include "resource/deserializer.hpp"
#include "shader/shader_compute.h"

class ShaderComputeDeserializer : public Deserializer {
public:
    ShaderComputeDeserializer() = default;
    virtual ~ShaderComputeDeserializer() = default;

    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) override;

    virtual std::string get_type() const override {
        return get_res_type<ShaderCompute>::VALUE;
    }
};

#endif
