// This file comes from my personal project (not published yet, so I can't provide a link).
#include "system/system.h"

#ifdef __linux__
#include "system/os_linux.h"
std::unique_ptr<System::Factory> System::factory =
    std::make_unique<LinuxOS::Factory>();
#elif defined _WIN32 || defined _WIN64
#include "system/os_windows.h"
std::unique_ptr<System::Factory> System::factory =
    std::make_unique<WindowsOS::Factory>();
#endif

System *System::get_singleton()
{
    if (singleton == nullptr)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (singleton == nullptr)
        {
            singleton = factory->create();
        }
    }
    return singleton;
}
