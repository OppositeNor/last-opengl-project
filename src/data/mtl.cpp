#include "data/mtl.h"
#include "utils.hpp"
#include <sstream>

MTL::MTL() {
    diffuse_map = nullptr;
    bump_map = nullptr;
    specular_map = nullptr;
}

MTL::~MTL() {
}

void MTL::parse(const char* file_path) {
    std::stringstream content(load_text_file(file_path));
    std::string line;

    while(std::getline(content, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        TextureConfigurations config;
        config.generate_mipmap = false;
        config.scale_nearest_min = true;
        config.scale_nearest_mag = true;
        if (type == "Ka") {
            ambient_color = glm::vec3();
            ss >> ambient_color->x >> ambient_color->y >> ambient_color->z;
        }
        else if (type == "Kd") {
            diffuse_color = glm::vec3();
            ss >> diffuse_color->x >> diffuse_color->y >> diffuse_color->z;
        }
        else if (type == "Ks") {
            specular_color = glm::vec3();
            ss >> specular_color->x >> specular_color->y >> specular_color->z;
        }
        else if (type == "Ke") {
            emission_color = glm::vec3();
            ss >> emission_color->x >> emission_color->y >> emission_color->z;
        }
        if (type == "map_Kd") {
            std::string dir = std::string(file_path).substr(0, std::string(file_path).find_last_of('/'));
            std::string map_Kd;
            ss >> map_Kd;
            map_Kd = dir + "/" + map_Kd;
            if (diffuse_map == nullptr) {
                diffuse_map = std::make_shared<Texture2DRGB>(config, map_Kd);
            }
            diffuse_map->load();
        }
        else if (type == "map_Bump") {
            std::string dir = std::string(file_path).substr(0, std::string(file_path).find_last_of('/'));
            std::string map_Bump;
            ss >> map_Bump;
            map_Bump = dir + "/" + map_Bump;
            if (bump_map == nullptr) {
                bump_map = std::make_shared<Texture2DRGB>(config, map_Bump);
            }
            bump_map->load();
        }
        else if (type == "map_Ks") {
            std::string dir = std::string(file_path).substr(0, std::string(file_path).find_last_of('/'));
            std::string map_Ks;
            ss >> map_Ks;
            map_Ks = dir + "/" + map_Ks;
            if (specular_map == nullptr) {
                specular_map = std::make_shared<Texture2DRGB>(config, map_Ks);
            }
            specular_map->load();
        }
    }
}
