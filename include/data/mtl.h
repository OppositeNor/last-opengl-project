#ifndef __MTL_H__
#define __MTL_H__
#include "texture/texture_2d_rgb.h"
#include <glm/glm.hpp>
#include <memory>
#include <optional>

class MTL {
public:
    MTL();
    virtual ~MTL();

    MTL(const MTL&) = delete;

    // Parse and stores the material properties from the given file
    void parse(const char* filename);

    // set the ambiant color
    void set_ambient_color(const glm::vec3& color) {
        ambient_color = color;
    }

    // get the ambiant color
    const std::optional<glm::vec3>& get_ambient_color() const {
        return ambient_color;
    }

    // set the diffuse color
    void set_diffuse_color(const glm::vec3& color) {
        diffuse_color = color;
    }

    // get the diffuse color
    const std::optional<glm::vec3>& get_diffuse_color() const {
        return diffuse_color;
    }

    // set the specular color
    void set_specular_color(const glm::vec3& color) {
        specular_color = color;
    }

    // get the specular color
    const std::optional<glm::vec3>& get_specular_color() const {
        return specular_color;
    }

    // set the emission color
    void set_emission_color(const glm::vec3& color) {
        emission_color = color;
    }

    // get the emission color
    const std::optional<glm::vec3>& get_emission_color() const {
        return emission_color;
    }

    // set the diffuse map
    void set_diffuse_map(std::shared_ptr<Texture2DRGB> texture) noexcept {
        diffuse_map = texture;
        texture = nullptr;
    }

    // get the diffuse map
    std::shared_ptr<Texture2D> get_diffuse_map() {
        return diffuse_map;
    }

    // set the bump map
    void set_bump_map(std::shared_ptr<Texture2DRGB> texture) noexcept {
        bump_map = texture;
        texture = nullptr;
    }

    // get the bump map
    std::shared_ptr<Texture2D> get_bump_map() {
        return bump_map;
    }

    // set the specular map
    void set_specular_map(std::shared_ptr<Texture2DRGB> texture) noexcept {
        specular_map = texture;
        texture = nullptr;
    }

    // get the specular map
    std::shared_ptr<Texture2D> get_specular_map() {
        return specular_map;
    }
private:
    std::optional<glm::vec3> ambient_color;
    std::optional<glm::vec3> diffuse_color;
    std::optional<glm::vec3> specular_color;
    std::optional<glm::vec3> emission_color;
    std::shared_ptr<Texture2DRGB> diffuse_map;
    std::shared_ptr<Texture2DRGB> bump_map;
    std::shared_ptr<Texture2DRGB> specular_map;
};

#endif
