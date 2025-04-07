#include "data/obj_model.h"
#include "data/mtl.h"
#include "material/material_phong.h"
#include "material/material_phong_color.h"
#include "utils.hpp"
#include "graphics.h"
#include <unordered_map>
#include <sstream>

OBJModel::OBJModel(unsigned int p_object_id, const std::string& p_path)
    : material(nullptr), vao(0), vbo(0), ebo(0), object_id(p_object_id) {
    path = p_path;
}

OBJModel::~OBJModel() {
    if (ebo != 0) {
        GL_CALL(glDeleteBuffers, 1, &ebo);
    }
    if (vbo != 0) {
        GL_CALL(glDeleteBuffers, 1, &vbo);
    }
    if (vao != 0) {
        GL_CALL(glDeleteVertexArrays, 1, &vao);
    }
}

void OBJModel::load() {
    parse(path);
}

struct Vertex {
    int position_index;
    int texcoord_index;
    int normal_index;
    Tuple<float, 3> tangent;
    std::string to_string() const {
        return std::to_string(position_index) + "/" + std::to_string(texcoord_index) + "/" + std::to_string(normal_index)
            + "/" + std::to_string(tangent.x) + "/" + std::to_string(tangent.y) + "/" + std::to_string(tangent.z);
    }
};

Vertex parse_vertex(const std::string& str);

struct Triangle {
    Vertex vertices[3];
};

Tuple<float, 3> calculate_tangent(const Triangle& p_triangle, const std::vector<Tuple<float, 3>>& p_vertices,
                                  const std::vector<Tuple<float, 2>>& p_texcoords) {
    Tuple<float, 3> edge1 = p_vertices[p_triangle.vertices[1].position_index]
        - p_vertices[p_triangle.vertices[0].position_index];
    Tuple<float, 3> edge2 = p_vertices[p_triangle.vertices[2].position_index]
        - p_vertices[p_triangle.vertices[0].position_index];
    Tuple<float, 2> deltaUV1 = p_texcoords[p_triangle.vertices[1].texcoord_index]
        - p_texcoords[p_triangle.vertices[0].texcoord_index];
    Tuple<float, 2> deltaUV2 = p_texcoords[p_triangle.vertices[2].texcoord_index]
        - p_texcoords[p_triangle.vertices[0].texcoord_index];
    Tuple<float, 3> tangent;
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    return tangent;
}

void OBJModel::parse(const std::string& filename) {
    std::stringstream content(load_text_file(filename));
    std::vector<Tuple<float, 3>> vertices;
    std::vector<Tuple<float, 3>> normals;
    std::vector<Tuple<float, 2>> texcoords;
    std::vector<Triangle> faces;

    std::string line;

    while(std::getline(content, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        if (type == "v") {
            vertices.push_back(Tuple<float, 3>());
            ss >> vertices.back().x >> vertices.back().y >> vertices.back().z;
        }
        else if (type == "vn") {
            normals.push_back(Tuple<float, 3>());
            ss >> normals.back().x >> normals.back().y >> normals.back().z;
        }
        else if (type == "vt") {
            texcoords.push_back(Tuple<float, 2>());
            ss >> texcoords.back().x >> texcoords.back().y;
        }
        else if (type == "f") {
            std::vector<std::string> face;
            std::string index;
            while (ss >> index) {
                face.push_back(index);
            }
            auto triangles_vertices = triangulate(face);
            for (std::size_t i = 0; i < triangles_vertices.size() / 3; ++i) {
                Triangle triangle_face;
                triangle_face.vertices[0] = parse_vertex(triangles_vertices[i * 3]);
                triangle_face.vertices[1] = parse_vertex(triangles_vertices[i * 3 + 1]);
                triangle_face.vertices[2] = parse_vertex(triangles_vertices[i * 3 + 2]);
                faces.push_back(triangle_face);
            }
        }
        else if (type == "mtllib") {
            std::string mtl_path;
            ss >> mtl_path;
            mtl_path = get_directory(std::string(filename)) + "/" + mtl_path;
            MTL mtl;
            mtl.parse(mtl_path.c_str());
            if (mtl.get_diffuse_map() != nullptr
             && mtl.get_bump_map() != nullptr
             && mtl.get_specular_map() != nullptr) {
                material = std::make_shared<MaterialPhong>(
                    std::vector(1, REQUIRE_NON_NULL(mtl.get_diffuse_map())),
                    std::vector(1, REQUIRE_NON_NULL(mtl.get_bump_map())),
                    std::vector(1, REQUIRE_NON_NULL(mtl.get_specular_map()))
                );
            }
            else {
                material = std::make_shared<MaterialPhongColor>(
                    UNPACK_OPTIONAL(mtl.get_diffuse_color()),
                    UNPACK_OPTIONAL(mtl.get_specular_color())
                );
            }
        }
    }

    m_indices.clear();
    m_vertices.clear();
    m_indices.reserve(faces.size());

    std::unordered_map<std::string, unsigned int> vertices_map;
    Tuple<float, VERTEX_SIZE> vertex;
    for (std::size_t i = 0; i < faces.size(); ++i) {
        auto tangent = calculate_tangent(faces[i], vertices, texcoords);
        faces[i].vertices[0].tangent = tangent;
        faces[i].vertices[1].tangent = tangent;
        faces[i].vertices[2].tangent = tangent;
        for (int j = 0; j < 3; ++j) {
            auto vertex_hash = faces[i].vertices[j].to_string();
            if (vertices_map.find(vertex_hash) == vertices_map.end()) {
                vertices_map[vertex_hash] = m_vertices.size();
                vertex[0] = vertices[faces[i].vertices[j].position_index].x;
                vertex[1] = vertices[faces[i].vertices[j].position_index].y;
                vertex[2] = vertices[faces[i].vertices[j].position_index].z;
                vertex[3] = normals[faces[i].vertices[j].normal_index].x;
                vertex[4] = normals[faces[i].vertices[j].normal_index].y;
                vertex[5] = normals[faces[i].vertices[j].normal_index].z;
                vertex[6] = texcoords[faces[i].vertices[j].texcoord_index].x;
                vertex[7] = texcoords[faces[i].vertices[j].texcoord_index].y;
                vertex[8] = tangent.x;
                vertex[9] = tangent.y;
                vertex[10] = tangent.z;
                m_vertices.push_back(vertex);
            }
            m_indices.push_back(vertices_map[vertex_hash]);
        }
    }
    if (vao != 0) {
        update_vertex_data();
    }
}

void OBJModel::draw() const {
    if (vao == 0) {
        init_vertex_data();
    }
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glDrawElements, GL_TRIANGLES, get_indices_count(), GL_UNSIGNED_INT, 0);
    GL_CALL(glBindVertexArray, 0);
}

void OBJModel::init_vertex_data() const {
    if (m_vertices.empty() || m_indices.empty()) {
        throw std::runtime_error("Model not loaded");
    }
    GL_CALL(glGenVertexArrays, 1, &vao);
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glGenBuffers, 1, &vbo);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, get_vertices_count() * sizeof(float),
            m_vertices.data(), GL_STATIC_DRAW);
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_POS);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Tuple<float, VERTEX_SIZE>),
            (void*)(VERTEX_POSITION_OFFSET * sizeof(float)));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_NORMAL);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Tuple<float, VERTEX_SIZE>),
            (void*)(VERTEX_NORMAL_OFFSET * sizeof(float)));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_UV);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Tuple<float, VERTEX_SIZE>),
            (void*)(VERTEX_TEXCOORD_OFFSET * sizeof(float)));
    GL_CALL(glEnableVertexAttribArray, VERTEX_LOCATION_TANGENT);
    GL_CALL(glVertexAttribPointer, VERTEX_LOCATION_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Tuple<float, VERTEX_SIZE>),
            (void*)(VERTEX_TANGENT_OFFSET * sizeof(float)));
    GL_CALL(glGenBuffers, 1, &ebo);
    GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ebo);
    GL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, get_indices_count() * sizeof(unsigned int),
            m_indices.data(), GL_STATIC_DRAW);
    GL_CALL(glBindVertexArray, 0);
}

void OBJModel::update_vertex_data() {
    if (vao == 0) {
        init_vertex_data();
        return;
    }
    GL_CALL(glBindVertexArray, vao);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
    GL_CALL(glBufferData, GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Tuple<float, VERTEX_SIZE>),
            m_vertices.data(), GL_STATIC_DRAW);
    GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ebo);
    GL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
            m_indices.data(), GL_STATIC_DRAW);
    GL_CALL(glBindVertexArray, 0);
}

Vertex parse_vertex(const std::string& str) {
    Vertex vertex;
    vertex.position_index = std::stoi(str.substr(0, str.find('/'))) - 1;
    vertex.texcoord_index = std::stoi(str.substr(str.find('/') + 1,
                                                 str.find_last_of('/') - str.find('/') - 1)) - 1;
    vertex.normal_index = std::stoi(str.substr(str.find_last_of('/') + 1)) - 1;
    return vertex;
}
