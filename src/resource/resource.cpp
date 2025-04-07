#include "resource/resource.h"
#include "resource/resource_unit.h"
#include "utils.hpp"
#include <mutex>
#include <thread>
#include <toml++/toml.hpp>
#include <fstream>

Resource::Resource() {
}

Resource::~Resource() {
    {
        for (auto& resource : resources) {
            delete resource.second;
        }
        resources.clear();
    }
}

void Resource::load_resources(const std::string& p_res_headers_path) {
    get_all_resource(p_res_headers_path);
}

void Resource::clear_resources() {
    for (auto& resource : resources) {
        delete resource.second;
    }
    resources.clear();
}

void Resource::get_all_resource(const std::string& p_res_headers_path) {
    std::ifstream headers(p_res_headers_path);
    if (!headers.is_open()) {
        throw std::runtime_error("Failed to open resource headers file " + p_res_headers_path);
    }
    std::string headers_str((std::istreambuf_iterator<char>(headers)), std::istreambuf_iterator<char>());
    toml::table headers_table;
    try {
        headers_table = toml::parse(headers_str);
    }
    catch (const toml::parse_error& e) {
        throw std::runtime_error("Failed to parse resource headers file " + p_res_headers_path + ":\n" + e.what());
    }
    for (const auto& section : headers_table) {
        auto section_data = headers_table[section.first];
        auto resource_id = removed_quotes(section.first);
        if (resources.find(resource_id) != resources.end()) {
            throw std::runtime_error("Resource " + resource_id + " already exists.");
        }
        if (resource_id == "include") {
            std::optional dependencies = section_data["dependencies"].as_array();
            if (!dependencies) {
                throw std::runtime_error("Resource include does not declare dependencies.");
            }
            for (const auto& dependency : **dependencies) {
                std::optional<std::string> dependency_str = dependency.value<std::string>();
                if (!dependency_str) {
                    throw std::runtime_error("Resource include dependency not found");
                }
                dependency_str = get_directory(p_res_headers_path) + "/" + removed_quotes(*dependency_str);
                if (!dependency_str->ends_with(".toml")) {
                    dependency_str = *dependency_str + ".toml";
                }
                get_all_resource(*dependency_str);
            }
            continue;
        }
        std::optional<std::string> type = section_data["type"].value<std::string>();
        if (!type) {
            throw std::runtime_error("Resource " + resource_id + " does not declare a type.");
        }
        auto it = deserializers.find(*type);
        if (it == deserializers.end()) {
            throw std::runtime_error("Resource " + resource_id + " does not have a deserializer with type: " + *type);
        }
        resources[resource_id] =
            it->second->deserialize(std::string(section.first.str()), headers_table);
    }
    dispatch_load();
}

struct GlobalJobsHandler {
    std::vector<UniversalResourceUnit*> global_jobs;
    mutable std::mutex global_jobs_mutex;

    UniversalResourceUnit* request_new_job() {
        if (global_jobs.empty()) {
            return nullptr;
        }
        std::lock_guard lock(global_jobs_mutex);
        if (global_jobs.empty()) {
            return nullptr;
        }
        auto* result = global_jobs.back();
        global_jobs.pop_back();
        return result;
    }
};

void thread_run_tasks(GlobalJobsHandler* global_job_handler) {
    while(true) {
        auto* job = global_job_handler->request_new_job();
        if (job == nullptr) {
            break;
        }
        job->get_data()->load();
    }
}

void Resource::dispatch_load() {
    GlobalJobsHandler jobs_handler;
    std::vector<UniversalResourceUnit*> main_tasks;
    for (auto& res : resources) {
        if (!res.second->get_data()->get_is_main_only()) {
            jobs_handler.global_jobs.push_back(res.second);
        }
        else {
            main_tasks.push_back(res.second);
        }
    }
    unsigned int thread_count = std::thread::hardware_concurrency() - 1;
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (unsigned int i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread(thread_run_tasks, &jobs_handler));
    }
    for (auto* main_task : main_tasks) {
        main_task->get_data()->load();
    }
    for (auto& thread : threads) {
        thread.join();
    }
}

void Resource::add_deserializer(std::shared_ptr<Deserializer> p_deserializer) {
    deserializers[p_deserializer->get_type()] = p_deserializer;
    p_deserializer = nullptr;
}

UniversalResourceUnit* Resource::get_resource(const std::string& p_id) const {
    auto it = resources.find(p_id);
    if (it == resources.end()) {
        return nullptr;
    }
    return it->second;
}

std::vector<std::string> Resource::get_resource_ids() const {
    std::vector<std::string> ids;
    {
        for (const auto& resource : resources) {
            ids.push_back(resource.first);
        }
    }
    return ids;
}
