#ifndef __TEXTURE_CUBE_DESERIALIZER_H__
#define __TEXTURE_CUBE_DESERIALIZER_H__
#include "resource/deserializer.hpp"
#include "texture/texture_cube.h"

class TextureCubeDeserializer : public Deserializer {
public:
    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) override;

    virtual std::string get_type() const override {
        return get_res_type<TextureCube>::VALUE;
    }
};

#endif
