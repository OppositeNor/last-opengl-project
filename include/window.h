#ifndef __WINDOW_H__
#define __WINDOW_H__
#include "keyboard_input_event_observer.h"
#include "texture/texture_2d_rgb.h"
#include "texture/texture_cube.h"
#include "window_resize_event_observer.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

/**
 * @brief Window property
 */
struct WindowProp {
    /**
     * @brief Width of the window
     */
    int width;

    /**
     * @brief Height of the window
     */
    int height;

    /**
     * @brief Whether the window is fullscreen
     */
    bool fullscreen;

    /**
     * @brief Title of the window
     */
    const char* title = "";

    /**
     * @brief Construct a new Window Prop object
     * 
     * @param width Width of the window
     * @param height Height of the window
     * @param full_screen Whether the window is fullscreen
     * @param title Title of the window
     */
    WindowProp(int p_width, int p_height, bool p_full_screen, const char* p_title)
        : width(p_width), height(p_height),
          fullscreen(p_full_screen), title(p_title) {}

    /**
     * @brief Construct a new Window Prop object
     */
    WindowProp()
        : WindowProp(640, 480, false, "") {}
};

class Window {
public:
    /**
     * @brief Construct a new Window object
     * 
     * @param prop Window property
     * @throws std::runtime_error if failed to initialize.
     */
    Window(const WindowProp& p_prop = WindowProp());
    virtual ~Window();
    
    /**
     * @brief Initialize the window
     */
    void init();

    /**
     * @brief Get the window property
     * 
     * @return WindowProp Window property
     */
    const WindowProp& get_prop() const { return prop; }

#ifdef USE_SDL
    /**
     * @brief Check if the window should close
     * 
     * @return true if the window should close
     * @return false if the window should not close
     */
    bool should_close() const { return is_should_close; }
#else
    /**
     * @brief Check if the window should close
     * 
     * @return true if the window should close
     * @return false if the window should not close
     */
    bool should_close() const;
#endif

    /**
     * @brief Called before rendering
     */
    void before_render();

    /**
     * @brief Called after rendering
     */
    void after_render();

    /**
     * @brief Add a keyboard input event observer
     * 
     * @param p_observer Keyboard input event observer
     */
    void add_key_event_observer(KeyboardInputEventObserver* p_observer) {
        keyboard_observers.push_back(p_observer);
    }

    /**
     * @brief Add a window resize event observer
     * 
     * @param p_observer Resize event observer
     */
    void add_window_resize_event_observer(WindowResizeEventObserver* p_observer) {
        resize_observers.push_back(p_observer);
    }

    /**
     * @brief Register a key to listen.
     * 
     * @param key The name of the registered key
     * @param code The keycode of the registered key
     */
    void set_listen_key(const std::string& p_key, int p_code);

    /**
     * @brief Get the default texture
     * 
     * @return std::shared_ptr<const Texture2DRGB> The default texture
     */
    static std::shared_ptr<const Texture2DRGB> get_default_texture_2d() { return default_texture_2d; }

    /**
     * @brief Get the default texture cube
     * 
     * @return std::shared_ptr<const TextureCube> The default texture cube
     */
    static std::shared_ptr<const TextureCube> get_default_texture_cube() { return default_texture_cube; }

private:
    WindowProp prop;
    std::vector<KeyboardInputEventObserver*> keyboard_observers;
    std::vector<WindowResizeEventObserver*> resize_observers;
    struct WindowData* window_data;
    bool is_should_close;

    std::unordered_map<int, std::string> key_map;

    void init_window_lib();
    void init_glad();
    void init_imgui();
    void init_key_bindes();
    void terminate();

    void poll_events();

    inline static std::shared_ptr<Texture2DRGB> default_texture_2d = nullptr;
    inline static std::shared_ptr<TextureCube> default_texture_cube = nullptr;

#ifndef USE_SDL
    inline static std::unordered_map<class GLFWwindow*, Window*> window_map;
    static void framebuffer_size_callback(GLFWwindow* p_window, int p_width, int p_height);
    static void key_callback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods);
#endif
};
#endif
