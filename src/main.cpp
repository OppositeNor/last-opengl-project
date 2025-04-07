#include "application/application_cornell_box.h"
#include "application/application_house_hutao.h"
#include "application/application_spartan.h"
#include "application/application_sponza.h"
#include "system/system.h"
#include <cstring>

int main(int p_argc, char** p_argv) {
    Application* app;
    if (p_argc > 1 && strcmp(p_argv[1], "morph") == 0) {
        app = new ApplicationHouseHutao();
    }
    else if (p_argc > 1 && strcmp(p_argv[1], "pbr") == 0) {
        app = new ApplicationSpartan();
    }
    else if (p_argc > 1 && strcmp(p_argv[1], "cornell") == 0){
        app = new ApplicationCornellBox();
    }
    else {
        app = new ApplicationSponza();
    }
    #ifdef USE_SDL
    // app->init("window", 1920, 1080);
    app->init("window", 2160, 1440);
    #else
    app->init("window", 1260, 720);
    #endif
    double last_time, current_time, delta_t = 0.01;
    current_time = System::get_singleton()->get_time();
    while (!app->window_should_close()) {
        app->tick(delta_t);
        last_time = current_time;
        current_time = System::get_singleton()->get_time();
        delta_t = current_time - last_time;
    }
    app->terminate();
    delete app;
    return 0;
}
