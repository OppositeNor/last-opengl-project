// This file comes from my personal project (not published yet, so I can't provide a link).
// The purpose of the class here is to make the program more adaptable for different platforms
// for methods that are not part of the C++ standard library. I currently only tested this
// on Linux and Windows (because I don't have a Mac device). If you have to compile this on
// Mac please contact me (sun.yimin@northeastern.edu, or oppositenormal@outlook.com)

#ifndef __OS_LINUX_H__
#define __OS_LINUX_H__
#include "system.h"
#include <optional>
/**
 * @brief Represents a Linux operating system.
 */
class LinuxOS : public System
{
protected:
    LinuxOS();

public:

    /**
     * @brief Factory class for creating LinuxOS instances.
     */
    class Factory : public System::Factory
    {
    public:
        /**
         * @brief Create a new LinuxOS instance.
         *
         * @return System* The new LinuxOS instance.
         */
        virtual System* create() const final override;
    };

    virtual ~LinuxOS() override;

    /**
     * @brief Get the memory usage of the program.
     *
     * @return std::string The memory usage of the program.
     */
    virtual std::string get_program_memory_usage() const override;

    /**
     * @brief Get the current time in seconds.
     *
     * @return double The current time in seconds.
     */
    virtual double get_time() const override;
    /**
     * @brief Get the directory of the executable.
     *
     * @return std::string The directory of the executable.
     */
    virtual std::string get_exe_directory() const override;
    /**
     * @brief Get the path of the executable.
     *
     * @return std::string The path of the executable.
     */
    virtual std::string get_exe_path() const override;
    /**
     * @brief Sleep for a given amount of time.
     *
     * @param p_seconds The amount of time to sleep in seconds.
     */
    virtual void sleep(double p_seconds) const override;
private:
    std::optional<std::string> exe_path;
};
#endif
