#ifndef __RESOURCE_H__
#define __RESOURCE_H__
#include "resource/resource_unit.h"
#include "resource/deserializer.hpp"
#include <unordered_map>
#include <memory>

/**
 * @brief The resource class. Handles loading resources from files.
 */
class Resource {
public:
    /**
     * @brief Construct a new Resource object
     */ 
    Resource();
    virtual ~Resource();

    /**
     * @brief Load resources from a file.
     * 
     * @param p_res_headers_path The path to the resource headers file.
     */
    virtual void load_resources(const std::string& p_res_headers_path);

    /**
     * @brief Clear all loaded resources.
     */
    virtual void clear_resources();

    /**
     * @brief Get a resource by its id.
     * 
     * @param p_id The id of the resource.
     * @return ResourceUnit* The resource.
     */
    virtual UniversalResourceUnit* get_resource(const std::string& p_id) const;

    /**
     * @brief Add a deserializer.
     * 
     * @param p_deserializer The deserializer to add.
     */
    virtual void add_deserializer(std::shared_ptr<Deserializer> p_deserializer);

    /**
     * @brief Get all resource ids.
     * 
     * @return std::vector<std::string> The resource ids.
     */
    virtual std::vector<std::string> get_resource_ids() const;

protected:

    void get_all_resource(const std::string& p_res_headers_path);
    std::unordered_map<std::string, UniversalResourceUnit*> resources;
    std::unordered_map<std::string, std::shared_ptr<Deserializer>> deserializers;
    std::string load_text_file(const std::string &p_path) const;

private:
    void dispatch_load();
};
#endif
