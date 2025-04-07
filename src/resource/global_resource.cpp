#include "resource/global_resource.h"
#include <algorithm>

GlobalResource::GlobalResource() {
    sub_resource = new Resource();
}

GlobalResource::~GlobalResource() {
    delete sub_resource;
}

void GlobalResource::load_resources(const std::string& p_res_headers_path) {
    is_loading = true;
    sub_resource->load_resources(p_res_headers_path);
    is_loading = false;
}

void GlobalResource::clear_resources() {
    if (is_loading) {
        throw std::runtime_error("Cannot clear resources while loading resources.");
    }
    sub_resource->clear_resources();
}

void GlobalResource::clear_all_resources() {
    if (is_loading) {
        throw std::runtime_error("Cannot clear resources while loading resources.");
    }
    sub_resource->clear_resources();
    resources.clear();
}

void GlobalResource::load_global_resources(const std::string& p_global_res_headers_path) {
    is_loading = true;
    Resource::load_resources(p_global_res_headers_path);
    is_loading = false;
}

UniversalResourceUnit* GlobalResource::get_resource(const std::string& p_id) const {
    {
        auto it = resources.find(p_id);
        if (it != resources.end()) {
            return it->second;
        }
    }
    return sub_resource->get_resource(p_id);
}


std::shared_ptr<GlobalResource> GlobalResource::get_singleton() {
    if (singleton == nullptr) {
        std::lock_guard lock(singleton_mutex);
        if (singleton == nullptr) {
            singleton = std::shared_ptr<GlobalResource>(new GlobalResource());
            return singleton;
        }
    }
    return singleton;
}

void GlobalResource::add_deserializer(std::shared_ptr<Deserializer> p_deserializer) noexcept {
    deserializers[p_deserializer->get_type()] = p_deserializer;
    sub_resource->add_deserializer(std::move(p_deserializer));
}

std::vector<std::string> GlobalResource::get_resource_ids() const {
    std::vector<std::string> ids = sub_resource->get_resource_ids();
    {
        for (const auto& resource : resources) {
            ids.push_back(resource.first);
        }
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}
