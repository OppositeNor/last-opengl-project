// This file comes from my personal project (not published yet, so I can't provide a link).
#ifdef __linux__
#include "system/os_linux.h"
#include "utils.hpp"
#include <ctime>
#include <fstream>
#include <sstream>
#include <unistd.h>
System* LinuxOS::Factory::create() const {
    return new LinuxOS();
}

LinuxOS::LinuxOS() {}
LinuxOS::~LinuxOS() {}

std::string LinuxOS::get_program_memory_usage() const {
    std::ifstream file("/proc/self/status");
    std::stringstream ss;
    std::string line, memory, unit;
    while (std::getline(file, line))
    {
        if (line.find("VmRSS") != std::string::npos)
        {
            ss << line;
            ss >> memory >> memory >> unit;
            return memory + " " + unit;
        }
    }
    return "Failed to get memory usage.";
}

double LinuxOS::get_time() const {
    static struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

std::string LinuxOS::get_exe_directory() const {
    return get_directory(get_exe_path());
}

std::string LinuxOS::get_exe_path() const {
    if (!exe_path.has_value()) {
        char buf[1024];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf));
        if (len == -1) {
            throw std::runtime_error("Failed to get the executable path.");
        }
        buf[len] = '\0';
        return std::string(buf);
    }
    return exe_path.value();
}

void LinuxOS::sleep(double p_seconds) const {
    struct timespec ts;
    ts.tv_sec = p_seconds;
    ts.tv_nsec = (p_seconds - ts.tv_sec) * 1e9;
    nanosleep(&ts, nullptr);
}

#elif _WIN32
#warning "Windows system is not tested because I don't have a Windows machine. Please test this project with a Linux machine."

#include "system/os_windows.h"
#include <windows.h>
#include <psapi.h>

System *WindowsOS::Factory::create() const {
    return new WindowsOS();
}

WindowsOS::WindowsOS() : System() { timeBeginPeriod(2); }

WindowsOS::~WindowsOS() { timeEndPeriod(2); }

std::string WindowsOS::get_program_memory_usage() const {
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return std::to_string(pmc.WorkingSetSize / 1024) + " KB";
}

double WindowsOS::get_time() const {
    return static_cast<double>(GetTickCount64()) / 1000.0;
}

void WindowsOS::sleep(double p_seconds) const {
    Sleep(static_cast<DWORD>(p_seconds * 1000.0));
}

std::string WindowsOS::get_exe_path() const {
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    return std::string(buffer);
}

std::string WindowsOS::get_exe_directory() const {
    std::string path = get_exe_path();
    return path.substr(0, path.find_last_of("\\/"));
}
#else
#error "The operating system is not supported. Please contact me (sun.yimin@northeastern.edu, or oppositenormal@outlook.com) if you have to use your platform to build."
#endif
