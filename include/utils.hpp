#ifndef __UTILS_H__
#define __UTILS_H__
#include <cstddef>
#include <map>
#include <stdexcept>
#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <optional>

#define REQUIRE_NON_NULL(p_ptr) require_non_null(p_ptr, #p_ptr)
#define REQUIRE_OPTIONAL(p_optional) require_optional(p_optional, #p_optional)
#define UNPACK_OPTIONAL(p_optional) (*REQUIRE_OPTIONAL(p_optional))

class Reflectable {
public:

    template <typename T>
    void set_value(const std::string& p_key, T&& p_value);

    template <typename T>
    const T& get_value(const std::string& p_key);

    template <typename T>
    void register_reflect(const std::string& p_key, T& p_value);
private:
    struct SerializedUnit {
        void* value;
        size_t type_hash;
    };

    std::map<std::string, SerializedUnit> serialize_map;
};

template <typename T>
inline void Reflectable::set_value(const std::string& p_key, T&& p_value) {
    auto iter = serialize_map.find(p_key);
    using RRT = std::remove_reference<T>;
    if (iter == serialize_map.end()) {
        throw std::runtime_error("Key: " + p_key + " is not serialized.");
    }
    if (iter->second.type_hash != typeid(RRT).hash_code()) {
        throw std::runtime_error("Key: " + p_key + "'s value type does not match.");
    }
    *static_cast<T*>(iter->second) = std::forward<T>(p_value);
}

template <typename T>
inline void Reflectable::register_reflect(const std::string& p_key, T& p_value) {
    serialize_map[p_key] = (SerializedUnit){.value = &p_value, .type = typeid(std::remove_reference<T>).hash_code()};
}

template <typename T>
inline const T& Reflectable::get_value(const std::string& p_key) {
    auto iter = serialize_map.find(p_key);
    if (iter == serialize_map.end()) {
        throw std::runtime_error("Key: " + p_key + " is not serialized.");
    }
    return *static_cast<T*>(iter->second);
}


template <typename T>
void print_vector(const std::vector<T>& vec, int stride = -1) {
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << ", ";
        if (stride != -1 && (i + 1) % stride == 0) {
            std::cout << std::endl;
        }
    }
}

template <typename T>
void print_array(const T* arr, size_t size, int stride = -1) {
    for (size_t i = 0; i < size; ++i) {
        std::cout << arr[i] << ", ";
        if (stride != -1 && (i + 1) % stride == 0) {
            std::cout << std::endl;
        }
    }
}

template <typename T> requires std::is_same_v<std::remove_reference_t<T>, std::nullptr_t>
consteval void require_non_null(T, const char*) {
    static_assert(false, "Pointer should not be null.");
}

template <typename T>
T* require_non_null(T* p_ptr, const std::string& p_ptr_name) {
    if (p_ptr == nullptr) {
        throw std::runtime_error("Pointer: " + p_ptr_name + " should not be null");
    }
    return p_ptr;
}

template <typename T>
std::shared_ptr<T> require_non_null(std::shared_ptr<T> p_ptr, const std::string& p_ptr_name) {
    if (p_ptr == nullptr) {
        throw std::runtime_error("Pointer: " + p_ptr_name + " should not be null");
    }
    return p_ptr;
}

template <typename T>
std::optional<T> require_optional(const std::optional<T> p_optional, const std::string& p_optional_name) {
    if (!p_optional.has_value()) {
        throw std::runtime_error("Optional: " + p_optional_name + " does not contain a value");
    }
    return p_optional;
}

inline std::string& remove_quotes(std::string& p_str) {
    while (p_str.size() > 2 && p_str[0] == '\"' && p_str[p_str.size() - 1] == '\"') {
        p_str = p_str.substr(1, p_str.size() - 2);
    }
    while (p_str.size() > 2 && p_str[0] == '\'' && p_str[p_str.size() - 1] == '\'') {
        p_str = p_str.substr(1, p_str.size() - 2);
    }
    return p_str;
}

inline std::string to_unix_style_path(const std::string& p_path) {
    std::string path = p_path;
    for (size_t i = 0; i < path.size(); ++i) {
        if (path[i] == '\\') {
            path[i] = '/';
        }
    }
    return path;
}

inline std::string to_unix_style_path(const char* p_path) {
    return to_unix_style_path(std::string(p_path));
}

inline std::string get_directory(const std::string& p_path) {
    size_t found = p_path.find_last_of("/\\");
    if (found == std::string::npos) {
        return "";
    }
    return p_path.substr(0, found);
}

inline std::string get_ext(const std::string& p_path) {
    size_t found = p_path.find_last_of(".");
    if (found == std::string::npos) {
        return "";
    }
    return p_path.substr(found + 1);
}

inline std::string get_file_name(const std::string& p_path) {
    size_t found = p_path.find_last_of("/\\");
    if (found == std::string::npos) {
        return "";
    }
    return p_path.substr(found + 1);
}

template <typename T>
inline std::string removed_quotes(const T& p_str) {
    std::string str = std::string(p_str);
    return remove_quotes(str);
}

template <typename T, size_t Size>
struct Tuple {
    T data[Size];
    T& operator[](size_t index) {
        return data[index];
    }

    operator const T*() const {
        return data;
    }

    operator T*() {
        return data;
    }
};

template <typename T>
struct Tuple<T, 3> {
    Tuple() {}

    Tuple(const T& p_x, const T& p_y, const T& p_z)
        : x(p_x), y(p_y), z(p_z) {}

    Tuple(T&& p_x, T&& p_y, T&& p_z) noexcept
        : x(std::move(p_x)), y(std::move(p_y)), z(std::move(p_z)) {}
    union {
        struct {
            T x, y, z;
        };
        T data[3];
    };
    T& operator[](size_t index) {
        return data[index];
    }

    operator const T*() const {
        return data;
    }

    operator T*() {
        return data;
    }
};

template <typename T>
struct Tuple<T, 2> {
    Tuple() {}

    Tuple(const T& p_x, const T& p_y)
        : x(p_x), y(p_y) {}

    Tuple(T&& p_x, T&& p_y) noexcept
        : x(std::move(p_x)), y(std::move(p_y)) {}
    union {
        struct {
            T x, y;
        };
        T data[2];
    };
    T& operator[](size_t index) {
        return data[index];
    }

    operator const T*() const {
        return data;
    }

    operator T*() {
        return data;
    }
};

template <typename T, size_t Size>
Tuple<T, Size> operator+(const Tuple<T, Size>& p_t1, const Tuple<T, Size>& p_t2) {
    Tuple<T, Size> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = p_t1[i] + p_t2[i];
    }
    return result;
}

template <typename T, size_t Size>
Tuple<T, Size>& operator+=(Tuple<T, Size>& p_t1, const Tuple<T, Size>& p_t2) {
    for (size_t i = 0; i < Size; ++i) {
        p_t1[i] += p_t2[i];
    }
    return p_t1;
}

template <typename T, size_t Size>
Tuple<T, Size> operator-(const Tuple<T, Size>& p_t1, const Tuple<T, Size>& p_t2) {
    Tuple<T, Size> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = p_t1[i] - p_t2[i];
    }
    return result;
}

template <typename T, size_t Size>
Tuple<T, Size> operator*(const Tuple<T, Size>& p_tuple, float p_scaler) {
    Tuple<T, Size> result;
    for (size_t i = 0; i < Size; ++i) {
        result[i] = p_tuple[i] * p_scaler;
    }
    return result;
}

inline std::string load_text_file(const char* file_path) {
    std::string content;
    std::ifstream file_stream(file_path, std::ios::in);
    if (!file_stream.is_open()) {
        std::cout << "Could not read file " << file_path << std::endl;
        exit(1);
    }
    std::string line = "";
    while (!file_stream.eof()) {
        std::getline(file_stream, line);
        content.append(line + "\n");
    }
    return content;
}

inline std::string load_text_file(const std::string& file_path) {
    return load_text_file(file_path.c_str());
}

template <typename T>
inline std::vector<T> triangulate(const std::vector<T>& face) {
    std::vector<T> triangles;
    for (std::size_t i = 1; i < face.size() - 1; ++i) {
        triangles.push_back(face[0]);
        triangles.push_back(face[i]);
        triangles.push_back(face[i + 1]);
    }
    return triangles;
}

struct RenderData {
    glm::mat4 proj_mat;
    glm::vec3 ambient_light = glm::vec3(0.3f, 0.3f, 0.3f);
    float far_plane = 20.0f;
    class Camera* camera = nullptr;
    class Framebuffer* depth_buffer = nullptr;
    std::unordered_set<const class PointLightObject*>* point_lights;
    std::unordered_set<const class SpotLightObject*>* spot_lights;
    std::optional<Framebuffer*> normal_buffer;
    bool direct_lighting_enabled = true;
    bool gi_enabled = false;
}; // TODO: move this somewhere else

inline const float box_vertices[] = {
    // top
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,

    // bottom
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // front
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // back
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // left
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    
    // right
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f
};

inline const float inverse_box_vertices[] = {
    // top
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // bottom
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,

    // front
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // back
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // left
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    
    // right
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f
};


inline std::string remove_ppm_comments(std::string str) {
    while (str.find('#') != std::string::npos) {
        size_t pos = str.find('#');
        size_t end = str.find('\n', pos);
        str.erase(pos, end - pos);
    }
    return str;
}

#define VERTEX_LOCATION_POS 0
#define VERTEX_LOCATION_NORMAL 1
#define VERTEX_LOCATION_UV 2
#define VERTEX_LOCATION_TANGENT 3
#define VERTEX_LOCATION_BITANGENT 4
#define VERTEX_LOCATION_BONE_ID 5
#define VERTEX_LOCATION_BONE_WEIGHT 6


template <typename T>
struct SceneNodeType {
    constexpr static bool is_drawable = std::is_base_of_v<class Drawable, T>;
    constexpr static bool is_animated_object = std::is_base_of_v<class AnimatedObject, T>;
    constexpr static bool is_point_light = std::is_base_of_v<class PointLightObject, T>;
    constexpr static bool is_spot_light = std::is_base_of_v<class SpotLightObject, T>;
};

#endif


