#ifndef __RESOURCE_UNIT_H__
#define __RESOURCE_UNIT_H__
#include "resource/resource_base.h"
#include "utils.hpp"
/**
 * @brief Resource unit class. This is the base class for all resource units.
 */
class ResourceUnit {
public:

    /**
     * @brief Construct a new Resource Unit object
     *
     * @param p_type The type of the resource.
     */
    ResourceUnit(const std::string& p_id, const std::string& p_type) noexcept
        : id(p_id), type(p_type) {}

    virtual ~ResourceUnit() = default;

    /**
     * @brief Get the type of the resource.
     *
     * @return ResourceType The type of the resource.
     */
    const std::string& get_type() const {
        return type;
    }

protected:
    std::string id;
    std::string type;
};

/**
 * @brief Universal resource unit class. This is a resource unit for universal resources.
 */
class UniversalResourceUnit : public ResourceUnit {
public:

    /**
     * @brief Construct a new Universal Resource Unit object
     *
     * @param p_id The id of the resource.
     * @param p_data The data of the resource.
     */
    UniversalResourceUnit(const std::string& p_id, std::shared_ptr<ResourceBase> p_data, const std::string& p_type) noexcept
    : ResourceUnit(p_id, p_type), data(REQUIRE_NON_NULL(p_data)) {}

    /**
     * @brief Get the data of the resource.
     *
     * @return T* The data of the resource.
     */
    std::shared_ptr<ResourceBase> get_data() const {
        return data;
    }

    virtual ~UniversalResourceUnit() override {
    }

protected:
    std::shared_ptr<ResourceBase> data;
};

#endif
