#ifndef __DESERIALIZER_HPP__
#define __DESERIALIZER_HPP__
#include "resource/resource_unit.h"

#include <mutex>
#include <string>
#include <toml++/toml.h>

/**
 * @brief The deserializer interface.
 */
class Deserializer {
public:
    Deserializer() = default;
    virtual ~Deserializer() = default;

    virtual UniversalResourceUnit* deserialize(const std::string& p_id, const toml::table& p_header_table) = 0;

    virtual std::string get_type() const = 0;

protected:
    static int get_obj_id() {
        std::lock_guard<std::mutex> lock(obj_id_mutex);
        unsigned int result = ++obj_id_assign;
        return result;
    }

private:
    inline static unsigned int obj_id_assign = 0;
    inline static std::mutex obj_id_mutex;
};

template<typename T>
inline T toml_get_required_value(const toml::table* p_table, const std::string& p_key) {
    auto section = p_table->get(p_key);
    if (!section) {
        throw std::runtime_error("The following key is required: " + p_key);
    }
    std::optional<T> temp = section->value<T>();
    if (!temp.has_value()) {
        throw std::runtime_error("The following key is required: " + p_key);
    }
    return *temp;
}

template<typename T>
inline T toml_get_optional_value(const toml::table* p_table, const std::string& p_key, const T& p_default) {
    auto section = p_table->get(p_key);
    if (!section) {
        return p_default;
    }
    std::optional<T> temp = section->value<T>();
    if (!temp.has_value()) {
        return p_default;
    }
    return *temp;
}

template<typename T>
inline std::optional<T> toml_get_optional_value(const toml::table* p_table, const std::string& p_key) {
    auto section = p_table->get(p_key);
    if (!section) {
        return std::optional<T>();
    }
    std::optional<T> temp = section->value<T>();
    if (!temp.has_value()) {
        return std::optional<T>();
    }
    return temp;
}
#endif
