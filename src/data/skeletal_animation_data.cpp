#include "data/skeletal_animation_data.h"
#include "MikuMikuFormats/Pmx.h"
#include "animation/bone_animation.h"
#include "animation/skeletal_animation.h"
#include "material/material_pbr.h"
#include "material/material_phong.h"
#include "texture/texture.hpp"
#include "texture/texture_2d_r.h"
#include "texture/texture_2d_rgb.h"
#include "texture/texture_2d_rgba.h"
#include "utils.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#if !__WIN32
    #include <MikuMikuFormats/Vmd.h>
#endif

glm::mat4 convert_matrix_to_glm_format(const aiMatrix4x4& p_from) {
    // learnopengl.com
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = p_from.a1; to[1][0] = p_from.a2; to[2][0] = p_from.a3; to[3][0] = p_from.a4;
    to[0][1] = p_from.b1; to[1][1] = p_from.b2; to[2][1] = p_from.b3; to[3][1] = p_from.b4;
    to[0][2] = p_from.c1; to[1][2] = p_from.c2; to[2][2] = p_from.c3; to[3][2] = p_from.c4;
    to[0][3] = p_from.d1; to[1][3] = p_from.d2; to[2][3] = p_from.d3; to[3][3] = p_from.d4;
    return to;
}

void SkeletalAnimationData::load() {
    if (info.is_separate) {
        parse(info.path.c_str(), info.animations_paths, info.mesh_z_up_to_y_up);
    }
    else {
        parse(info.path.c_str(), info.mesh_z_up_to_y_up);
    }
}

void SkeletalAnimationData::parse(const char* p_path, bool p_mesh_z_up_to_y_up) {
    REQUIRE_NON_NULL(p_path);
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(p_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("Failed to load skeleton animation data at path: " + std::string(p_path)
                                 + ", error message: " + importer.GetErrorString());
    }
    bone_map = std::make_shared<std::map<std::string, int>>();
    SADStructData data;
    data.dir = get_directory(p_path);
    glm::mat3 transform = glm::mat3(1.0);
    if (p_mesh_z_up_to_y_up) {
        transform = glm::mat3(1.0, 0.0,  0.0,
                              0.0, 0.0, -1.0,
                              0.0, 1.0,  0.0);
    }
    data.vertex_transform = transform;
    data.normal_transform = glm::transpose(glm::inverse(data.vertex_transform));


    parse_model(scene, data);
    assert(data.bones_infos.size() == data.bones_offsets.size());
    parse_animator(scene, data);
    int anim_max_bone_id = data.bones_infos.size();
    parse_skeleton(scene, data);

    TextureConfigurations config;
    config.generate_mipmap = false;
    config.scale_nearest_min = true;
    config.scale_nearest_mag = true;
    config.srgb = false;
    std::unique_ptr<Texture2DFRGBA> bone_offsets = std::make_unique<Texture2DFRGBA>(config);

    bone_offsets->create_empty_texture(4, anim_max_bone_id);
    for (std::size_t i = 0; i < data.bones_offsets.size(); ++i) {
        bone_offsets->set_texture_row(i, &(data.bones_offsets[i][0][0]));
    }
    skeleton_model = std::make_shared<SkeletonModel>(std::move(data.meshes), bone_offsets.release(), object_id);
    animator_skeletal = std::make_shared<AnimatorSkeletal>(std::move(data.animations));
    skeleton = std::make_shared<Skeleton>(data.root_bone_index, std::move(data.bones_infos), anim_max_bone_id);
    importer.FreeScene();
}

void SkeletalAnimationData::parse(const char* p_model_path, const std::vector<std::string>& p_animations_paths, bool p_mesh_z_up_to_y_up) {
    REQUIRE_NON_NULL(p_model_path);
    bone_map = std::make_shared<std::map<std::string, int>>();
    SADStructData data;
    data.mesh_z_up_to_y_up = p_mesh_z_up_to_y_up;
    data.dir = get_directory(p_model_path);
    glm::mat3 transform = glm::mat3(1.0);
    if (p_mesh_z_up_to_y_up) {
        transform = glm::mat3(1.0, 0.0,  0.0,
                              0.0, 0.0, -1.0,
                              0.0, 1.0,  0.0);
    }
    data.vertex_transform = transform;
    data.normal_transform = glm::transpose(glm::inverse(data.vertex_transform));

    parse_assimp(p_model_path, &SkeletalAnimationData::parse_model, data);
    int anim_max_bone_id;
    for (auto& path : p_animations_paths) {
        if (get_ext(path) == "vmd") {
            #if !_WIN32
            parse_assimp(p_model_path, &SkeletalAnimationData::parse_skeleton, data);
            parse_vmd_animator(path.c_str(), p_model_path, data);
            anim_max_bone_id = data.bones_infos.size();
            #else
            throw std::runtime_error("MMD currently not supported for Windows.");
            #endif
        }
        else {
            parse_assimp(path.c_str(), &SkeletalAnimationData::parse_animator, data);
            anim_max_bone_id = data.bones_infos.size();
            parse_assimp(p_model_path, &SkeletalAnimationData::parse_skeleton, data);
        }
    }
    TextureConfigurations config;
    config.generate_mipmap = false;
    config.scale_nearest_min = true;
    config.scale_nearest_mag = true;
    config.srgb = false;
    std::unique_ptr<Texture2DFRGBA> bone_offsets = std::make_unique<Texture2DFRGBA>(config);
    bone_offsets->create_empty_texture(4, anim_max_bone_id);
    for (int i = 0; i < anim_max_bone_id; ++i) {
        bone_offsets->set_texture_row(i, &(data.bones_offsets[i][0][0]));
    }
    skeleton_model = std::make_shared<SkeletonModel>(std::move(data.meshes), bone_offsets.release(), object_id);
    animator_skeletal = std::make_shared<AnimatorSkeletal>(std::move(data.animations));
    skeleton = std::make_shared<Skeleton>(data.root_bone_index, std::move(data.bones_infos), anim_max_bone_id);
}

void SkeletalAnimationData::parse_animator(const aiScene* p_scene, SADStructData& p_data) {
    p_data.animations.reserve(p_scene->mNumAnimations);
    for (std::size_t i = 0; i < p_scene->mNumAnimations; ++i) {
        parse_animation(p_scene->mAnimations[i], p_data);
    }
}

void SkeletalAnimationData::parse_animation(const aiAnimation* p_animation, SADStructData& p_data) {
    std::vector<BoneAnimation> bones_animations(p_data.bones_infos.size(), BoneAnimation());
    std::vector<SingleMeshAnimation> morph_animations;
    morph_animations.reserve(p_animation->mNumMorphMeshChannels);
    p_data.bones_animations = &bones_animations;
    p_data.morph_animations = &morph_animations;

    for (std::size_t i = 0; i < p_animation->mNumChannels; ++i) {
        auto* channel = p_animation->mChannels[i];
        std::string bone_name = channel->mNodeName.data;
        parse_bone_animation(bone_name, channel, p_data);
    }
    for (std::size_t i = 0; i < p_animation->mNumMorphMeshChannels; ++i) {
        auto* channel = p_animation->mMorphMeshChannels[i];
        std::string mesh_name = channel->mName.data;
        parse_morph_animation(mesh_name, channel, p_data);
    }
    p_data.animations.push_back(SkeletalAnimation(p_animation->mName.data,
                                                  std::move(bones_animations),
                                                  std::move(morph_animations),
                                                  p_animation->mDuration,
                                                  p_animation->mTicksPerSecond));
    p_data.bones_animations = nullptr;
    p_data.morph_animations = nullptr;
}

void SkeletalAnimationData::parse_bone_animation(const std::string& bone_name, const aiNodeAnim* channel, SADStructData& p_data) {
    auto bone_id_iter = bone_map->find(bone_name);
    int bone_id;
    if (bone_id_iter == bone_map->end()) {
        bone_id = p_data.bones_infos.size();
        bone_map->emplace(bone_name, bone_id);
        Skeleton::BoneInfo bone_info;
        bone_info.id = bone_id;
        bone_info.bone_name = bone_name;
        p_data.bones_infos.push_back(bone_info);
        p_data.bones_animations->push_back(BoneAnimation());
    }
    else {
        bone_id = bone_id_iter->second;
    }

    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
    positions.reserve(channel->mNumPositionKeys);
    for (std::size_t i = 0; i < channel->mNumPositionKeys; ++i) {
        KeyPosition key_pos;
        aiVector3D ai_pos = channel->mPositionKeys[i].mValue;
        key_pos.position = glm::vec3(ai_pos.x, ai_pos.y, ai_pos.z);
        key_pos.time_stamp = channel->mPositionKeys[i].mTime;
        positions.push_back(key_pos);
    }
    rotations.reserve(channel->mNumRotationKeys);
    for (std::size_t i = 0; i < channel->mNumRotationKeys; ++i) {
        KeyRotation key_rot;
        aiQuaternion ai_orient = channel->mRotationKeys[i].mValue;
        key_rot.orientation = glm::quat(ai_orient.w, ai_orient.x, ai_orient.y, ai_orient.z);
        key_rot.time_stamp = channel->mRotationKeys[i].mTime;
        rotations.push_back(key_rot);
    }
    scales.reserve(channel->mNumScalingKeys);
    for (std::size_t i = 0; i < channel->mNumScalingKeys; ++i) {
        KeyScale key_scale;
        aiVector3D ai_scale = channel->mScalingKeys[i].mValue;
        key_scale.scale = glm::vec3(ai_scale.x, ai_scale.y, ai_scale.z);
        key_scale.time_stamp = channel->mScalingKeys[i].mTime;
        scales.push_back(key_scale);
    }
    (*p_data.bones_animations)[bone_id] = BoneAnimation(bone_name, bone_id, std::move(positions), std::move(rotations), std::move(scales));
}

void SkeletalAnimationData::parse_morph_animation(const std::string& p_mesh_name, const aiMeshMorphAnim* p_channel, SADStructData& p_data) {
    SingleMeshAnimation result;
    for (auto& mesh : mesh_map) {
        if (p_mesh_name.starts_with(mesh.first)) {
            result.mesh_indices = mesh.second;
            break;
        }
    }
    std::map<unsigned int, unsigned int> mesh_index_to_result_index;
    for (unsigned int i = 0; i < p_channel->mNumKeys; ++i) {
        auto* key = &(p_channel->mKeys[i]);
        parse_morph_key(key, mesh_index_to_result_index, result);
    }
    p_data.morph_animations->push_back(result);
}

void SkeletalAnimationData::parse_morph_key(aiMeshMorphKey* p_key,
                                            std::map<unsigned int, unsigned int>& p_mesh_index_to_result_index,
                                            SingleMeshAnimation& p_mesh) {
    for (unsigned int i = 0; i < p_key->mNumValuesAndWeights; ++i) {
        int index;
        auto iter = p_mesh_index_to_result_index.find(p_key->mValues[i]);
        if (iter == p_mesh_index_to_result_index.end()) {
            index = p_mesh.attach_mesh_animations.size();
            p_mesh_index_to_result_index[p_key->mValues[i]] = index;
            p_mesh.attach_mesh_animations.push_back(AttachMeshAnimation());
            p_mesh.attach_mesh_animations[index].index = p_key->mValues[i];
        }
        else {
            index = iter->second;
        }
        p_mesh.attach_mesh_animations[index].values.push_back(p_key->mWeights[i]);
        p_mesh.attach_mesh_animations[index].times.push_back(p_key->mTime);
    }
}

void SkeletalAnimationData::parse_model(const aiScene* p_scene, SADStructData& p_data) {
    process_model_node(p_scene->mRootNode, p_scene, p_data);
}

void SkeletalAnimationData::process_model_node(const aiNode* p_node, const aiScene* p_scene, SADStructData& p_data) {
    p_data.meshes.reserve(p_node->mNumMeshes);
    for (std::size_t i = 0; i < p_node->mNumMeshes; ++i) {
        aiMesh* mesh = p_scene->mMeshes[p_node->mMeshes[i]];
        mesh_map[mesh->mName.data].push_back(p_data.meshes.size());
        auto result_mesh = process_model_mesh(mesh, p_scene, p_data);
        p_data.meshes.push_back(result_mesh);
    }
    for (std::size_t i = 0; i < p_node->mNumChildren; ++i) {
        process_model_node(p_node->mChildren[i], p_scene, p_data);
    }
}

template<typename TextureType>
std::vector<std::shared_ptr<Texture2D>> load_textures(const aiMaterial* p_material, aiTextureType p_type, const std::string& p_dir,
                                                    std::vector<std::shared_ptr<Texture2D>>& p_textures,
                                                    std::map<std::string, unsigned int>& texture_indices, bool p_is_srgb) {
    std::size_t texture_count = p_material->GetTextureCount(p_type);
    std::vector<std::shared_ptr<Texture2D>> result(texture_count);
    for (std::size_t i = 0; i < texture_count; ++i) {
        aiString path;
        if (p_material->GetTexture(p_type, i, &path) != AI_SUCCESS) {
            throw std::runtime_error("Failed to load texture at path: " + std::string(path.C_Str()));
        }
        auto it = texture_indices.find(path.C_Str());
        if (it != texture_indices.end()) {
            result[i] = p_textures[it->second];
        } else {
            TextureConfigurations config;
            config.generate_mipmap = true;
            config.scale_nearest_min = false;
            config.scale_nearest_mag = false;
            config.srgb = p_is_srgb;
            std::shared_ptr texture = std::make_shared<TextureType>(config);
            texture->parse_img((p_dir + "/" + path.C_Str()).c_str());
            texture_indices[path.C_Str()] = p_textures.size();
            p_textures.push_back(texture);
            result[i] = texture;
        }
    }
    return result;
}

SkeletonModel::Mesh* SkeletalAnimationData::process_model_mesh(const aiMesh* p_mesh, const aiScene* p_scene, SADStructData& p_data) {
    if (p_mesh->mNumAnimMeshes == 0) {
        return process_model_mesh_static(p_mesh, p_scene, p_data);
    }
    else {
        return process_model_mesh_morph(p_mesh, p_scene, p_data);
    }
}

void get_vertex_data(SkeletonModel::Mesh::Vertex& p_vertex, const aiMesh* p_mesh, int p_vert_index,
                     const glm::mat3& p_transform, const glm::mat3& p_transform_normal);

SkeletonModel::Mesh* SkeletalAnimationData::process_model_mesh_static(const aiMesh* p_mesh, const aiScene* p_scene, SADStructData& p_data) {
    std::unique_ptr<SkeletonModel::StaticMesh> result = std::make_unique<SkeletonModel::StaticMesh>();
    process_model_vertices(p_mesh, result->vertices, result->indices, p_data);

    if ((int)p_mesh->mMaterialIndex >= 0) {
        aiMaterial* material = p_scene->mMaterials[p_mesh->mMaterialIndex];
        result->material = process_material(material, p_data);
    }
    process_model_bones(p_mesh, result->vertices, p_data);
    return result.release();
}

SkeletonModel::Mesh* SkeletalAnimationData::process_model_mesh_morph(const aiMesh* p_mesh, const aiScene* p_scene, SADStructData& p_data) {
    std::unique_ptr<SkeletonModel::MorphAnimMesh> result = std::make_unique<SkeletonModel::MorphAnimMesh>();
    process_model_vertices(p_mesh, result->default_vertices, result->indices, p_data);
    result->morph_anims_keys.resize(p_mesh->mNumAnimMeshes);
    for (unsigned int i = 0; i < p_mesh->mNumAnimMeshes; ++i) {
        process_model_vertices(p_mesh->mAnimMeshes[i], result->morph_anims_keys[i].vertices, result->default_vertices, p_data);
        result->morph_anims_keys[i].weight = 0.0;
    }

    if ((int)p_mesh->mMaterialIndex >= 0) {
        aiMaterial* material = p_scene->mMaterials[p_mesh->mMaterialIndex];
        result->material = process_material(material, p_data);
    }
    process_model_bones(p_mesh, result->default_vertices, p_data);
    return result.release();
}

void SkeletalAnimationData::process_model_vertices(const aiMesh* p_mesh, std::vector<SkeletonModel::Mesh::Vertex>& p_vertices,
                                                   std::vector<unsigned int>& p_indices, SADStructData& p_data) {
    p_vertices.resize(p_mesh->mNumVertices);
    for (std::size_t i = 0; i < p_mesh->mNumVertices; ++i) {
        SkeletonModel::Mesh::Vertex vertex;
        get_vertex_data(vertex, p_mesh, i, p_data.vertex_transform, p_data.normal_transform);
        p_vertices[i] = vertex;
    }
    for (std::size_t i = 0; i < p_mesh->mNumFaces; ++i) {
        aiFace face = p_mesh->mFaces[i];
        for (std::size_t j = 0; j < face.mNumIndices; ++j) {
            p_indices.push_back(face.mIndices[j]);
        }
    }
}

void SkeletalAnimationData::process_model_vertices(const aiAnimMesh* p_mesh, std::vector<SkeletonModel::MorphKeyData>& p_vertices,
                                                   const std::vector<SkeletonModel::Mesh::Vertex>& p_default_vertices, SADStructData& p_data) {
    p_vertices.resize(p_mesh->mNumVertices);
    for (std::size_t i = 0; i < p_mesh->mNumVertices; ++i) {
        SkeletonModel::MorphKeyData vertex;
        glm::vec3 vec;
        vec.x = p_mesh->mVertices[i].x;
        vec.y = p_mesh->mVertices[i].y;
        vec.z = p_mesh->mVertices[i].z;
        vec = p_data.vertex_transform * vec;
        vertex.position.x = vec.x;
        vertex.position.y = vec.y;
        vertex.position.z = vec.z;
        vertex.position = vertex.position - p_default_vertices[i].position;
        vec.x = p_mesh->mNormals[i].x;
        vec.y = p_mesh->mNormals[i].y;
        vec.z = p_mesh->mNormals[i].z;
        vec = p_data.normal_transform * vec;
        vertex.normal.x = vec.x;
        vertex.normal.y = vec.y;
        vertex.normal.z = vec.z;
        vertex.normal = p_default_vertices[i].normal - vertex.normal;
        if (p_mesh->mTextureCoords[0]) {
            vertex.texture_uv.x = p_mesh->mTextureCoords[0][i].x;
            vertex.texture_uv.y = p_mesh->mTextureCoords[0][i].y;
            vertex.texture_uv = vertex.texture_uv - p_default_vertices[i].texture_uv;
        }
        if (p_mesh->mTangents != nullptr) {
            vec.x = p_mesh->mTangents[i].x;
            vec.y = p_mesh->mTangents[i].y;
            vec.z = p_mesh->mTangents[i].z;
            vec = p_data.normal_transform * vec;
            vertex.tangent.x = vec.x;
            vertex.tangent.y = vec.y;
            vertex.tangent.z = vec.z;
            vertex.tangent = vertex.tangent - p_default_vertices[i].tangent;
            vec.x = p_mesh->mBitangents[i].x;
            vec.y = p_mesh->mBitangents[i].y;
            vec.z = p_mesh->mBitangents[i].z;
            vec = p_data.normal_transform * vec;
            vertex.bitangent.x = vec.x;
            vertex.bitangent.y = vec.y;
            vertex.bitangent.z = vec.z;
            vertex.bitangent = vertex.bitangent - p_default_vertices[i].bitangent;
        }
        p_vertices[i] = vertex;
    }
}

void get_vertex_data(SkeletonModel::Mesh::Vertex& p_vertex, const aiMesh* p_mesh, int p_vert_index,
                     const glm::mat3& p_transform, const glm::mat3& p_transform_normal) {
    glm::vec3 vec;
    vec.x = p_mesh->mVertices[p_vert_index].x;
    vec.y = p_mesh->mVertices[p_vert_index].y;
    vec.z = p_mesh->mVertices[p_vert_index].z;
    vec = p_transform * vec;
    p_vertex.position.x = vec.x;
    p_vertex.position.y = vec.y;
    p_vertex.position.z = vec.z;
    vec.x = p_mesh->mNormals[p_vert_index].x;
    vec.y = p_mesh->mNormals[p_vert_index].y;
    vec.z = p_mesh->mNormals[p_vert_index].z;
    vec = p_transform_normal * vec;
    p_vertex.normal.x = vec.x;
    p_vertex.normal.y = vec.y;
    p_vertex.normal.z = vec.z;
    if (p_mesh->mTextureCoords[0]) {
        p_vertex.texture_uv.x = p_mesh->mTextureCoords[0][p_vert_index].x;
        p_vertex.texture_uv.y = p_mesh->mTextureCoords[0][p_vert_index].y;
    }
    vec.x = p_mesh->mTangents[p_vert_index].x;
    vec.y = p_mesh->mTangents[p_vert_index].y;
    vec.z = p_mesh->mTangents[p_vert_index].z;
    vec = p_transform_normal * vec;
    p_vertex.tangent.x = vec.x;
    p_vertex.tangent.y = vec.y;
    p_vertex.tangent.z = vec.z;
    vec.x = p_mesh->mBitangents[p_vert_index].x;
    vec.y = p_mesh->mBitangents[p_vert_index].y;
    vec.z = p_mesh->mBitangents[p_vert_index].z;
    vec = p_transform_normal * vec;
    p_vertex.bitangent.x = vec.x;
    p_vertex.bitangent.y = vec.y;
    p_vertex.bitangent.z = vec.z;
    for (std::size_t j = 0; j < SkeletonModel::MAX_BONE_INFLUENCE; ++j) {
        p_vertex.bone_ids[j] = -1;
        p_vertex.bone_weights[j] = 0;
    }
}


Material* SkeletalAnimationData::process_material(const aiMaterial* p_material, SADStructData& p_data) {
    std::vector<std::shared_ptr<Texture2D>> albedo_map;
    std::vector<std::shared_ptr<Texture2D>> normal_map;
    std::vector<std::shared_ptr<Texture2D>> metalic_map;
    std::vector<std::shared_ptr<Texture2D>> roughness_map;
    std::vector<std::shared_ptr<Texture2D>> ao_map;
    albedo_map = load_textures<Texture2DRGBA>(p_material, aiTextureType_BASE_COLOR, p_data.dir, p_data.textures, p_data.texture_indices, true);
    normal_map = load_textures<Texture2DRGB>(p_material, aiTextureType_NORMALS, p_data.dir, p_data.textures, p_data.texture_indices, false);
    metalic_map = load_textures<Texture2DR>(p_material, aiTextureType_METALNESS, p_data.dir, p_data.textures, p_data.texture_indices, false);
    roughness_map = load_textures<Texture2DR>(p_material, aiTextureType_DIFFUSE_ROUGHNESS, p_data.dir, p_data.textures, p_data.texture_indices, false);
    ao_map = load_textures<Texture2DR>(p_material, aiTextureType_AMBIENT_OCCLUSION, p_data.dir, p_data.textures, p_data.texture_indices, false);
    if (!metalic_map.empty()) {
        return new MaterialPBR(albedo_map, normal_map, metalic_map, roughness_map, ao_map);
    }
    std::vector<std::shared_ptr<Texture2D>> diffuse_map;
    std::vector<std::shared_ptr<Texture2D>> specular_map;
    diffuse_map = load_textures<Texture2DRGBA>(p_material, aiTextureType_DIFFUSE, p_data.dir, p_data.textures, p_data.texture_indices, true);
    specular_map = load_textures<Texture2DRGB>(p_material, aiTextureType_SPECULAR, p_data.dir, p_data.textures, p_data.texture_indices, true);
    return new MaterialPhong(diffuse_map, specular_map, normal_map);
}

void SkeletalAnimationData::process_model_bones(const aiMesh* p_mesh, std::vector<SkeletonModel::Mesh::Vertex>& p_vertices, SADStructData& p_data) {
    for (std::size_t bone_index = 0; bone_index < p_mesh->mNumBones; ++bone_index) {
        auto bone = p_mesh->mBones[bone_index];
        std::string bone_name = bone->mName.C_Str();
        int bone_id;
        if (bone_map->find(bone_name) == bone_map->end()) {
            auto bone_offset = bone->mOffsetMatrix;
            auto bone_offset_mat4 = convert_matrix_to_glm_format(bone_offset);
            bone_id = p_data.bones_infos.size();
            Skeleton::BoneInfo bone_info;

            bone_info.id = bone_id;
            bone_info.bone_name = bone_name;
            bone_info.children_ids = std::vector<int>();
            bone_info.node_transform = glm::mat4(1.0);
            p_data.bones_infos.push_back(bone_info);
            p_data.bones_offsets.push_back(bone_offset_mat4);

            (*bone_map)[bone_name] = bone_id;
        }
        else {
            bone_id = (*bone_map)[bone_name];
        }
        for (std::size_t weight_index = 0; weight_index < bone->mNumWeights; ++weight_index) {
            assign_bone_to_vertex(p_mesh, weight_index, bone_index, bone_id, p_vertices);
        }
    }
}

void SkeletalAnimationData::assign_bone_to_vertex(const aiMesh* p_mesh, int p_weight_index, int p_bone_index,
                                                  int p_bone_id, std::vector<SkeletonModel::Mesh::Vertex>& p_vertices) {
    auto& weight = p_mesh->mBones[p_bone_index]->mWeights[p_weight_index];
    if (weight.mWeight <= 0.0) {
        return;
    }

    bool full = true;
    assert(weight.mVertexId < p_vertices.size());
    SkeletonModel::Mesh::Vertex& vertex = p_vertices[weight.mVertexId];
    for (std::size_t i = 0; i < SkeletonModel::MAX_BONE_INFLUENCE; ++i) {
        if (vertex.bone_ids[i] == -1) {
            vertex.bone_ids[i] = p_bone_id;
            vertex.bone_weights[i] = weight.mWeight;
            full = false;
            break;
        }
    }
    if (full) {
        int min_index = 0;
        for (std::size_t i = 1; i < SkeletonModel::MAX_BONE_INFLUENCE; ++i) {
            if (vertex.bone_weights[i] < vertex.bone_weights[min_index]) {
                min_index = i;
            }
        }
        if (weight.mWeight > vertex.bone_weights[min_index]) {
            vertex.bone_ids[min_index] = p_bone_id;
            vertex.bone_weights[min_index] = weight.mWeight;
        }
    }
}

void SkeletalAnimationData::parse_skeleton(const aiScene* p_scene, SADStructData& p_data) {
    auto root_node = p_scene->mRootNode;
    parse_skeleton_node(root_node, p_data);
    int bone_id = bone_map->find(root_node->mName.C_Str())->second;
    p_data.root_bone_index = bone_id;
}

Skeleton::BoneInfo* SkeletalAnimationData::parse_skeleton_node(const aiNode* p_node, SADStructData& p_data) {
    auto bone_id_iter = bone_map->find(p_node->mName.data);
    int bone_id;
    if (bone_id_iter == bone_map->end()) {
        bone_id = p_data.bones_infos.size();
        bone_map->emplace(p_node->mName.data, bone_id);
        Skeleton::BoneInfo info;
        info.id = bone_id;
        p_data.bones_infos.push_back(info);
    }
    else {
        bone_id = bone_id_iter->second;
    }
    p_data.bones_infos[bone_id].node_transform = convert_matrix_to_glm_format(p_node->mTransformation);
    p_data.bones_infos[bone_id].children_ids.reserve(p_node->mNumChildren);
    for (unsigned int i = 0; i < p_node->mNumChildren; ++i) {
        auto child_node = parse_skeleton_node(p_node->mChildren[i], p_data);
        p_data.bones_infos[bone_id].children_ids.push_back(child_node->id);
    }
    return &(p_data.bones_infos[bone_id]);
}

void SkeletalAnimationData::parse_assimp(const char* p_path, std::function<void(SkeletalAnimationData*, const aiScene*, SADStructData&)> p_func,
                                         SADStructData& p_data) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(p_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || !scene->mRootNode) {
        throw std::runtime_error("Failed to load skeleton animation data at path: " + std::string(p_path)
                               + ", error message: " + importer.GetErrorString());
    }
    p_func(this, scene, p_data);
    importer.FreeScene();
}

#if !_WIN32
void SkeletalAnimationData::parse_vmd_animator(const char* p_path, const char* p_model_path, SADStructData& p_data) {
    std::unique_ptr<vmd::VmdMotion> motion = vmd::VmdMotion::LoadFromFile(p_path);
    process_vmd_animation(p_model_path, get_file_name(p_path), motion.get(), p_data);
}

void SkeletalAnimationData::process_vmd_animation(const std::string& p_model_path, const std::string& p_anim_name, const vmd::VmdMotion* p_motion, SADStructData& p_data) {
    std::map<int, std::vector<KeyPosition>> pos_keys;
    std::map<int, std::vector<KeyRotation>> rot_keys;
    int max_frame = -1;

    std::filebuf fb;
    if(!fb.open(p_model_path, std::ios::in | std::ios::binary)) {
        throw std::runtime_error("Failed to open file: " + p_model_path);
    }
    std::istream is(&fb);

    pmx::PmxModel model;
    model.Read(&is);
    if (model.model_name != p_motion->model_name) {
        std::cout << "Warning: model name does not match. Model: " << model.model_name
            << ", Motion: " << p_motion->model_name << std::endl;
    }
    std::map<utfstring, int> pmx_bone_map;
    for (int i = 0; i < model.bone_count; ++i) {
        pmx_bone_map[model.bones[i].bone_name] = i;
    }
    for (auto& bone_frame : p_motion->bone_frames) {
        auto iter = bone_map->find(bone_frame.name);
        int bone_id;
        if (iter == bone_map->end()) {
            bone_id = p_data.bones_infos.size();
            Skeleton::BoneInfo bone_info;
            bone_info.bone_name = bone_frame.name;
            bone_info.id = bone_id;
            p_data.bones_infos.push_back(bone_info);
            p_data.bones_offsets.push_back(glm::mat4(1.0));
            bone_map->emplace(bone_frame.name, bone_id);
        }
        else {
            bone_id = iter->second;
        }
        KeyPosition pos;
        pos.position.x = bone_frame.position[0];
        pos.position.y = bone_frame.position[1];
        pos.position.z = bone_frame.position[2];
        pos.position += glm::vec3(p_data.bones_infos[bone_id].node_transform * glm::vec4(0.0, 0.0, 0.0, 1.0));
        pos.time_stamp = (double)bone_frame.frame / 30.0;
        KeyRotation rot;
        rot.orientation.x = -bone_frame.orientation[0];
        rot.orientation.y = -bone_frame.orientation[1];
        rot.orientation.z = bone_frame.orientation[2];
        rot.orientation.w = bone_frame.orientation[3];
        rot.time_stamp = (double)bone_frame.frame / 30.0;
        pos_keys[bone_id].push_back(pos);
        rot_keys[bone_id].push_back(rot);
        if (bone_frame.frame > max_frame) {
            max_frame = bone_frame.frame;
        }
    }
    if (max_frame == -1) {
        return;
    }
    std::vector<BoneAnimation> bone_animations(p_data.bones_infos.size());
    for (int i = 0; i < static_cast<int>(bone_animations.size()); ++i) {
        if (pos_keys.find(i) != pos_keys.end()) {
            auto bone_pos_keys = std::move(pos_keys[i]);
            auto bone_rot_keys = std::move(rot_keys[i]);
            std::sort(bone_pos_keys.begin(), bone_pos_keys.end(), [](const KeyPosition& p_pos_1, const KeyPosition& p_pos_2) {
                return p_pos_1.time_stamp < p_pos_2.time_stamp;
            });
            std::sort(bone_rot_keys.begin(), bone_rot_keys.end(), [](const KeyRotation& p_rot_1, const KeyRotation& p_rot_2) {
                return p_rot_1.time_stamp < p_rot_2.time_stamp;
            });
            bone_animations[i] = BoneAnimation(
                p_data.bones_infos[i].bone_name, i,
                std::move(bone_pos_keys),
                std::move(bone_rot_keys),
                std::vector<KeyScale>());
        }
    }
    p_data.animations.push_back(SkeletalAnimation(p_anim_name, std::move(bone_animations),
                                                  std::vector<SingleMeshAnimation>(),
                                                  (double)max_frame / 30.0, 1.0));
    fb.close();
}
#endif