// This file comes from my personal project (not published yet, so I can't provide a link).
// The purpose of the class here is to make the program more adaptable for different platforms
// for methods that are not part of the C++ standard library. I currently only tested this
// on Linux and Windows (because I don't have a Mac device). If you have to compile this on
// Mac please contact me (sun.yimin@northeastern.edu, or oppositenormal@outlook.com)

#ifndef __OS_WINDOWS_H__
#define __OS_WINDOWS_H__
#include "system.h"

/**
 * @brief Windows operating system class
 *
 */
class WindowsOS : public System
{
protected:
    WindowsOS();

public:

    /**
     * @brief Factory class for creating WindowsOS instances.
     */
    class Factory : public System::Factory
    {
    public:
        /**
         * @brief Create a new WindowsOS instance.
         *
         * @return System* The new WindowsOS instance.
         */
        virtual System* create() const final override;
    };

    virtual ~WindowsOS() override;

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
     * @brief Sleep for a certain amount of time.
     *
     * @param p_seconds The amount of time to sleep in seconds.
     */
    virtual void sleep(double p_seconds) const override;

    /**
     * @brief Get the path to the exe file.
     *
     * @return std::string The path to the exe file.
     */
    virtual std::string get_exe_path() const override;

    /**
     * @brief Get the path to the directory of the exe file.
     *
     * @return std::string The path to the directory of the exe file.
     */
    virtual std::string get_exe_directory() const override;
};
#endif
