#ifndef __TEXTURE_2D_RGBA_DESERIALIZER_H__
#define __TEXTURE_2D_RGBA_DESERIALIZER_H__
#include "resource/deserializer.hpp"
#include "texture/texture_2d_rgba.h"

class Texture2DRGBADeserializer : public Deserializer {
public:
    Texture2DRGBADeserializer() = default;
    virtual ~Texture2DRGBADeserializer() = default;

    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) override;

    virtual std::string get_type() const override {
        return get_res_type<Texture2DRGBA>::VALUE;
    }

};


#endif
