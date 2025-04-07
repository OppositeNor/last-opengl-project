// This file comes from my personal project (not published yet, so I can't provide a link).
// The purpose of the class here is to make the program more adaptable for different platforms
// for methods that are not part of the C++ standard library. I currently only tested this
// on Linux and Windows (because I don't have a Mac device). If you have to compile this on
// Mac please contact me (sun.yimin@northeastern.edu, or oppositenormal@outlook.com)

#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include <memory>
#include <mutex>
#include <string>

/**
 * @brief System class. This handles all system related operations that are
 * not part of the standard library.
 */
class System
{
public:
    class Factory;
protected:
    System() = default;
    inline static System *singleton = nullptr;
    inline static std::mutex mutex;

    static std::unique_ptr<Factory> factory;

public:

    class Factory
    {
    public:
        Factory() = default;
        virtual ~Factory() = default;
        virtual System *create() const = 0;
    };

    virtual ~System() {}

    /**
     * @brief Get the singleton instance of the system.
     *
     * @return System* The singleton instance of the system.
     */
    static System *get_singleton();

    /**
     * @brief Get the memory usage of the program.
     *
     * @return std::string The memory usage of the program.
     */
    virtual std::string get_program_memory_usage() const = 0;

    /**
     * @brief Get the current time in seconds.
     *
     * @return double The current time in seconds.
     */
    virtual double get_time() const = 0;

    /**
     * @brief Sleep for a certain amount of time.
     *
     * @param p_seconds The amount of time to sleep in seconds.
     */
    virtual void sleep(double p_seconds) const = 0;

    /**
     * @brief Get the path of the executable.
     *
     * @return std::string The path of the executable.
     */
    virtual std::string get_exe_path() const = 0;

    /**
     * @brief Get the directory of the executable.
     *
     * @return std::string The directory of the executable.
     */
    virtual std::string get_exe_directory() const = 0;
};
#endif
