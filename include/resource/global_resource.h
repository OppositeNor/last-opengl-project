#ifndef __GLOBAL_RESOURCE_H__
#define __GLOBAL_RESOURCE_H__
#include "resource/resource.h"
#include "resource/resource_base.h"
#include "resource/resource_unit.h"

/**
 * @brief Global resource. Contains some resources that are shared between all resources, and
 * won't be cleared when clearing resources.
 */
class GlobalResource : public Resource {
public:

    /**
     * @brief The path to the global resource headers file.
     */
    inline static const std::string GLOBAL_RES_HEADERS_PATH = "global_resources.toml";

    virtual ~GlobalResource() override;

    /**
     * @brief Load resources from a file.
     * 
     * @param p_res_headers_path The path to the resource headers file.
     */
    virtual void load_resources(const std::string& p_res_headers_path) override;

    /**
     * @brief Clear all loaded resources except the global resource.
     */
    virtual void clear_resources() override;

    /**
     * @brief Clear all loaded resources including the global resource.
     */
    virtual void clear_all_resources();

    /**
     * @brief Load global resources.
     */
    void load_global_resources(const std::string& p_global_res_headers_path);

    /**
     * @brief Get a resource by its id and cast it to a specific type.
     * 
     * @tparam T The type to cast the resource to.
     * @param p_id The id of the resource.
     * @return T* The resource casted to the specific type.
     * @throw std::runtime_error If the resource is not of the specific type.
     */
    template <typename T>
    std::shared_ptr<T> get_resource(const std::string& p_id) const {
        auto resource = get_resource(p_id);
        if (resource == nullptr) {
            return nullptr;
        }
        if (get_res_type<T>::VALUE != get_resource(p_id)->get_type()) {
            throw std::runtime_error("Resource " + p_id + " is not of type " + get_res_type<T>::VALUE + ".");
        }
        return std::dynamic_pointer_cast<T>(get_resource(p_id)->get_data());
    }

    /**
     * @brief Get a resource by its id.
     * 
     * @param p_id The id of the resource.
     * @return ResourceUnit* The resource.
     */
    virtual UniversalResourceUnit* get_resource(const std::string& p_id) const override;

    /**
     * @brief Get the singleton instance of the global resource.
     * 
     * @return std::shared_ptr<GlobalResource> The singleton instance of the global resource.
     */
    static std::shared_ptr<GlobalResource> get_singleton();

    /**
     * @brief Add a deserializer.
     * 
     * @param p_deserializer The deserializer to add.
     */
    virtual void add_deserializer(std::shared_ptr<Deserializer> p_deserializer) noexcept override;

    /**
     * @brief Get all resource ids.
     * 
     * @return std::vector<std::string> The resource ids.
     */
    virtual std::vector<std::string> get_resource_ids() const override;
private:

    GlobalResource();
    Resource* sub_resource;
    bool is_loading = false;

    inline static std::shared_ptr<GlobalResource> singleton = nullptr;
    inline static std::mutex singleton_mutex;
};
#endif
