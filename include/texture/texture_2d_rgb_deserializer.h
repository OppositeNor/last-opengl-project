#ifndef __TEXTURE_2D_RGB_DESERIALIZER_H__
#define __TEXTURE_2D_RGB_DESERIALIZER_H__
#include "resource/deserializer.hpp"
#include "texture/texture_2d_rgb.h"

class Texture2DRGBDeserializer : public Deserializer {
public:
    Texture2DRGBDeserializer() = default;
    virtual ~Texture2DRGBDeserializer() = default;

    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) override;

    virtual std::string get_type() const override {
        return get_res_type<Texture2DRGB>::VALUE;
    }

};


#endif
