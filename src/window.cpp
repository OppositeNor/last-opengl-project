#include "window.h"
#include "graphics.h"
#include "resource/global_resource.h"
#include "system/system.h"
#include <stdexcept>
#include <imgui.h>
#ifdef USE_SDL
#if USE_LOCAL_DEPS
#include <SDL.h>
#include <SDL_video.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#endif
#include <backends/imgui_impl_sdl2.h>
#else
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#endif

#include <backends/imgui_impl_opengl3.h>

#ifdef USE_SDL
struct WindowData {
    SDL_Window* window_instance;
    SDL_GLContext gl_context;
};
#else
struct WindowData {
    GLFWwindow* window_instance;
};
#endif

Window::Window(const WindowProp& p_prop) : prop(p_prop) {
    is_should_close = false;
    window_data = new WindowData();
    init();
}

Window::~Window() {
    terminate();
    delete window_data;
}

void Window::init() {
    init_window_lib();
    init_glad();
    init_imgui();
    if (default_texture_2d == nullptr) {
        default_texture_2d = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<Texture2DRGB>("default_texture_2d"));
    }
    if (default_texture_cube == nullptr) {
        for (int i = 0; i < 6; ++i) {
            default_texture_cube = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<TextureCube>("default_texture_cube"));
        }
    }
    init_key_bindes();
}

#ifdef USE_SDL
void Window::init_window_lib() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window_data->window_instance = SDL_CreateWindow(prop.title,
                                                    SDL_WINDOWPOS_UNDEFINED,
                                                    SDL_WINDOWPOS_UNDEFINED,
                                                    prop.width, prop.height,
                                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if (window_data->window_instance == nullptr) {
        throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
    }

    if (prop.fullscreen) {
        SDL_SetWindowFullscreen(window_data->window_instance, SDL_WINDOW_FULLSCREEN);
    }

    window_data->gl_context = SDL_GL_CreateContext(window_data->window_instance);
    if (window_data->gl_context == nullptr) {
        throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
    }
}

void Window::init_glad() {
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        throw std::runtime_error("Failed to initialize GLAD");
    }

#ifndef NDEBUG
    void GLAPIENTRY debug_callback(GLenum p_source, GLenum p_type, GLuint p_id, GLenum p_severity,
                                   GLsizei p_length, const GLchar* p_message, const void* p_userParam);

    GL_CALL(glEnable, GL_DEBUG_OUTPUT);
    GL_CALL(glEnable, GL_CULL_FACE);
    GL_CALL(glDebugMessageControl, GL_DONT_CARE, GL_DEBUG_TYPE_PERFORMANCE, GL_DEBUG_SEVERITY_NOTIFICATION,
            0, NULL, GL_FALSE);
    GL_CALL(glDebugMessageCallback, debug_callback, nullptr);
#endif

    int max_group_size[3];
    max_group_size[0] = get_max_compute_workgroup_size(0);
    max_group_size[1] = get_max_compute_workgroup_size(1);
    max_group_size[2] = get_max_compute_workgroup_size(2);
    std::cout << "Compute shader max group size: " << max_group_size[0] << "x"
                                                   << max_group_size[1] << "x"
                                                   << max_group_size[2] << std::endl;
}

void Window::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplSDL2_InitForOpenGL(window_data->window_instance, window_data->gl_context);
    ImGui_ImplOpenGL3_Init();
}

void Window::init_key_bindes() {
    set_listen_key("camera_forward", SDLK_w);
    set_listen_key("camera_backward", SDLK_s);
    set_listen_key("camera_left", SDLK_a);
    set_listen_key("camera_right", SDLK_d);
    set_listen_key("camera_up", SDLK_e);
    set_listen_key("camera_down", SDLK_q);
    set_listen_key("camera_rotate_pitch_up", SDLK_UP);
    set_listen_key("camera_rotate_pitch_down", SDLK_DOWN);
    set_listen_key("camera_rotate_yaw_left", SDLK_LEFT);
    set_listen_key("camera_rotate_yaw_right", SDLK_RIGHT);
    set_listen_key("change_render_mode", SDLK_c);
    set_listen_key("toggle_draw_skeleton", SDLK_n);
    set_listen_key("toggle_ui", SDLK_f);

    set_listen_key("light_forward", SDLK_i);
    set_listen_key("light_backward", SDLK_k);
    set_listen_key("light_left", SDLK_j);
    set_listen_key("light_right", SDLK_l);
    set_listen_key("light_up", SDLK_u);
    set_listen_key("light_down", SDLK_o);
    set_listen_key("switch_test_plane_display_map", SDLK_3);
    set_listen_key("toggle_direct_light", SDLK_t);
    set_listen_key("toggle_gi", SDLK_y);
}

void Window::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(window_data->gl_context);
    SDL_DestroyWindow(window_data->window_instance);
    window_data->window_instance = nullptr;
    SDL_Quit();
    default_texture_2d = nullptr;
    default_texture_cube = nullptr;
}

void Window::before_render() {
    poll_events();
    SDL_GL_SwapWindow(window_data->window_instance);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Window::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                is_should_close = true;
                break;
            case SDL_KEYDOWN:
                for (auto observer : keyboard_observers) {
                    if (event.key.repeat == 0) {
                        observer->on_key_pressed(key_map[static_cast<int>(event.key.keysym.sym)]);
                    }
                }
                break;
            case SDL_KEYUP:
                for (auto observer : keyboard_observers) {
                    observer->on_key_released(key_map[static_cast<int>(event.key.keysym.sym)]);
                }
                break;
        }
    }
}
#else

bool Window::should_close() const {
    return glfwWindowShouldClose(window_data->window_instance);
}

void Window::init_window_lib() {
    if (glfwInit() != GLFW_TRUE) {
        const char* description = nullptr;
        glfwGetError(&description);
        throw std::runtime_error("Failed to initialize GLFW: " + std::string(description));
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window_data->window_instance = glfwCreateWindow(prop.width, prop.height, prop.title,
                                                    prop.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    if (window_data->window_instance == nullptr) {
        const char* description = nullptr;
        glfwGetError(&description);
        throw std::runtime_error("Failed to create window: " + std::string(description));
    }
    window_map[window_data->window_instance] = this;

    glfwMakeContextCurrent(window_data->window_instance);
}
void Window::init_glad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        throw std::runtime_error("Failed to initialize GLAD");
    }
#ifndef NDEBUG
    void GLAPIENTRY debug_callback(GLenum p_source, GLenum p_type, GLuint p_id, GLenum p_severity,
                                   GLsizei p_length, const GLchar* p_message, const void* p_userParam);

    GL_CALL(glEnable, GL_DEBUG_OUTPUT);
    GL_CALL(glDebugMessageControl, GL_DONT_CARE, GL_DEBUG_TYPE_PERFORMANCE, GL_DEBUG_SEVERITY_NOTIFICATION,
                          0, NULL, GL_FALSE);
    GL_CALL(glDebugMessageCallback, debug_callback, nullptr);
#endif
    glfwSetFramebufferSizeCallback(window_data->window_instance, framebuffer_size_callback);
    glfwSetKeyCallback(window_data->window_instance, key_callback);

    int max_group_size[3];
    max_group_size[0] = get_max_compute_workgroup_size(0);
    max_group_size[1] = get_max_compute_workgroup_size(1);
    max_group_size[2] = get_max_compute_workgroup_size(2);
    std::cout << "Compute shader max group size: " << max_group_size[0] << "x"
                                                   << max_group_size[1] << "x"
                                                   << max_group_size[2] << std::endl;
}
void Window::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    if (!ImGui_ImplGlfw_InitForOpenGL(window_data->window_instance, true)) {
        throw std::runtime_error("Failed to initialize ImGui, ImGui_ImplGlfw_InitForOpenGL call failed.");
    }
    if (!ImGui_ImplOpenGL3_Init()) {
        throw std::runtime_error("Failed to initialize ImGui, ImGui_ImplOpenGL3_Init call failed.");
    }
}

void Window::init_key_bindes() {
    set_listen_key("camera_forward", GLFW_KEY_W);
    set_listen_key("camera_backward", GLFW_KEY_S);
    set_listen_key("camera_left", GLFW_KEY_A);
    set_listen_key("camera_right", GLFW_KEY_D);
    set_listen_key("camera_up", GLFW_KEY_E);
    set_listen_key("camera_down", GLFW_KEY_Q);
    set_listen_key("camera_rotate_pitch_up", GLFW_KEY_UP);
    set_listen_key("camera_rotate_pitch_down", GLFW_KEY_DOWN);
    set_listen_key("camera_rotate_yaw_left", GLFW_KEY_LEFT);
    set_listen_key("camera_rotate_yaw_right", GLFW_KEY_RIGHT);
    set_listen_key("change_render_mode", GLFW_KEY_C);
    set_listen_key("toggle_draw_skeleton", GLFW_KEY_N);
    set_listen_key("toggle_ui", GLFW_KEY_F);

    set_listen_key("light_forward", GLFW_KEY_I);
    set_listen_key("light_backward", GLFW_KEY_K);
    set_listen_key("light_left", GLFW_KEY_J);
    set_listen_key("light_right", GLFW_KEY_L);
    set_listen_key("light_up", GLFW_KEY_U);
    set_listen_key("light_down", GLFW_KEY_O);
}

void Window::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_data->window_instance);
    window_data->window_instance = nullptr;
    glfwTerminate();
    default_texture_2d = nullptr;
    delete default_texture_cube;
    default_texture_cube = nullptr;
}
void Window::before_render() {
    poll_events();
    glfwSwapBuffers(window_data->window_instance);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::poll_events() {
    glfwPollEvents();
}

void Window::framebuffer_size_callback(GLFWwindow* p_window, int p_width, int p_height) {
    auto* window = window_map[p_window];
    for (auto* observers : window->resize_observers) {
        observers->on_resize(p_width, p_height);
    }
}

void Window::key_callback(GLFWwindow* p_window, int p_key, int, int p_action, int) {
    auto* window = window_map[p_window];
    auto iter = window->key_map.find(p_key);
    if (iter == window->key_map.end()) {
        return;
    }
    const std::string& key = iter->second;
    for (auto* observer : window->keyboard_observers) {
        if (p_action == GLFW_PRESS) {
            observer->on_key_pressed(key);
        }
        if (p_action == GLFW_RELEASE) {
            observer->on_key_released(key);
        }
    }
}

#endif

void Window::after_render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::set_listen_key(const std::string& key, int code) {
    key_map[code] = key;
}

#ifndef NDEBUG
void GLAPIENTRY debug_callback(GLenum, GLenum p_type, GLuint, GLenum p_severity,
                               GLsizei, const GLchar* p_message, const void*) {
    if (p_severity >= GL_DEBUG_SEVERITY_HIGH) {
        std::stringstream ss;
        std::cout << "GL Debug: type: " << std::hex << p_type << " severity: " << p_severity
            << std::dec << "\nmessage: " << p_message << std::endl;
    }
}
#endif

