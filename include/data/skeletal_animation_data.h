#ifndef __SKELETAL_ANIMATION_DATA_H__
#define __SKELETAL_ANIMATION_DATA_H__
#include "animation/animator_skeletal.h"
#include "data/skeleton.h"
#include "data/skeleton_model.h"
#include "material/material.hpp"
#include "resource/resource_base.h"
#include <functional>

class aiNode;
class aiScene;
class aiMesh;
class aiAnimMesh;
class aiAnimation;
class aiMeshMorphAnim;
class aiMeshMorphKey;
class aiNodeAnim;
class aiMaterial;
namespace vmd {
class VmdMotion;
};

template<>
struct get_res_type<class SkeletalAnimationData> {
    constexpr static const char* VALUE = "skeletal_animation_data";
};

class SkeletalAnimationData : public ResourceBase {
public:

    struct SkeletonAnimationDataInfo {
        bool mesh_z_up_to_y_up;
        bool is_separate;
        std::string path;
        std::vector<std::string> animations_paths;
    };

    SkeletalAnimationData(unsigned int p_object_id, const SkeletonAnimationDataInfo& p_info)
        : object_id(p_object_id), info(p_info) {
    }

    virtual void load() override;

    virtual ~SkeletalAnimationData() {

    }

    std::shared_ptr<SkeletonModel> get_skeleton_model() {
        return skeleton_model;
    }

    std::shared_ptr<const SkeletonModel> get_skeleton_model() const {
        return skeleton_model;
    }

    std::shared_ptr<Skeleton> get_skeleton() {
        return skeleton;
    }

    std::shared_ptr<const Skeleton> get_skeleton() const {
        return skeleton;
    }

    std::shared_ptr<AnimatorSkeletal> get_animator_skeletal() {
        return animator_skeletal;
    }

    std::shared_ptr<const AnimatorSkeletal> get_animator_skeletal() const{
        return animator_skeletal;
    }

private:

    void parse(const char* p_path, bool p_mesh_z_up_to_y_up = false);

    void parse(const char* p_model_path, const std::vector<std::string>& p_animations_paths, bool p_mesh_z_up_to_y_up = false);

    std::shared_ptr<SkeletonModel> skeleton_model;
    std::shared_ptr<AnimatorSkeletal> animator_skeletal;
    std::shared_ptr<Skeleton> skeleton;

    std::shared_ptr<std::map<std::string, int>> bone_map;
    std::map<std::string, std::vector<unsigned int>> mesh_map;

    struct SADStructData {
        std::string dir;
        std::vector<SkeletonModel::Mesh*> meshes;
        std::vector<glm::mat4> bones_offsets;

        std::map<std::string, unsigned int> texture_indices;
        std::vector<std::shared_ptr<Texture2D>> textures;
        std::vector<Skeleton::BoneInfo> bones_infos;
    
        std::vector<SkeletalAnimation> animations;
        std::vector<BoneAnimation>* bones_animations;
        std::vector<SingleMeshAnimation>* morph_animations;

        glm::mat3 vertex_transform;
        glm::mat3 normal_transform;

        int root_bone_index;
        bool mesh_z_up_to_y_up;
   };

    void parse_animator(const aiScene* p_scene, SADStructData& p_data);
    void parse_model(const aiScene* p_scene, SADStructData& p_data);
    void parse_skeleton(const aiScene* p_scene, SADStructData& p_data);

    void parse_animation(const aiAnimation* p_animation, SADStructData& p_data);
    void parse_bone_animation(const std::string& p_bone_name, const aiNodeAnim* p_channel, SADStructData& p_data);
    void parse_morph_animation(const std::string& p_mesh_name, const aiMeshMorphAnim* p_channel, SADStructData& p_data);
    void parse_morph_key(aiMeshMorphKey* p_key, std::map<unsigned int, unsigned int>& p_mesh_index_to_result_index, SingleMeshAnimation& p_mesh);

    void process_model_node(const aiNode* p_node, const aiScene* p_scene, SADStructData& p_data);
    SkeletonModel::Mesh* process_model_mesh(const aiMesh* p_mesh, const aiScene* p_scene, SADStructData& p_data);
    SkeletonModel::Mesh* process_model_mesh_static(const aiMesh* p_mesh, const aiScene* p_scene, SADStructData& p_data);
    SkeletonModel::Mesh* process_model_mesh_morph(const aiMesh* p_mesh, const aiScene* p_scene, SADStructData& p_data);
    void process_model_vertices(const aiMesh* p_mesh, std::vector<SkeletonModel::Mesh::Vertex>& p_vertices,
                                std::vector<unsigned int>& p_indices, SADStructData& p_data);
    void process_model_vertices(const aiAnimMesh* p_mesh, std::vector<SkeletonModel::MorphKeyData>& p_vertices,
                                const std::vector<SkeletonModel::Mesh::Vertex>& p_default_vertices, SADStructData& p_data);
    Material* process_material(const aiMaterial* p_material, SADStructData& p_data);
    void process_model_bones(const aiMesh* mesh, std::vector<SkeletonModel::Mesh::Vertex>& p_vertices, SADStructData& p_data);
    void assign_bone_to_vertex(const aiMesh* p_bone, int weight_index, int bone_index, int bone_id, std::vector<SkeletonModel::Mesh::Vertex>& p_vertices);

    const aiNode* find_root_bone(const aiNode* p_node);
    Skeleton::BoneInfo* parse_skeleton_node(const aiNode* p_node, SADStructData& p_data);
    void parse_assimp(const char* p_path, std::function<void(SkeletalAnimationData*, const aiScene*, SADStructData&)> p_func,
                      SADStructData& p_data);
    void parse_vmd_animator(const char* p_path, const char* p_model_path, SADStructData& p_data);
    void process_vmd_animation(const std::string& p_model_path, const std::string& p_anim_name, const vmd::VmdMotion* p_motion, SADStructData& p_data);

    unsigned int object_id;
    SkeletonAnimationDataInfo info;
};

#endif
