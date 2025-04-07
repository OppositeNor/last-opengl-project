#ifndef __SKELETAL_ANIMATION_DATA_DESERIALIZER_H__
#define __SKELETAL_ANIMATION_DATA_DESERIALIZER_H__

#include "data/skeletal_animation_data.h"
#include "resource/deserializer.hpp"

class SkeletalAnimationDataDeserializer : public Deserializer {
public:
    SkeletalAnimationDataDeserializer() = default;
    virtual ~SkeletalAnimationDataDeserializer() override = default;

    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) override;

    virtual std::string get_type() const override {
        return get_res_type<SkeletalAnimationData>::VALUE;
    }
};

#endif
