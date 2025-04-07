#ifndef __OBJ_MODEL_DESERIALIZER_H__
#define __OBJ_MODEL_DESERIALIZER_H__
#include "resource/deserializer.hpp"
#include "data/obj_model.h"

class OBJModelDeserializer : public Deserializer {
public:
    OBJModelDeserializer() = default;
    virtual ~OBJModelDeserializer() = default;

    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) override;

    virtual std::string get_type() const override {
        return get_res_type<OBJModel>::VALUE;
    }
};

#endif
