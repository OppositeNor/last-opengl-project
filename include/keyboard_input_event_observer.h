#ifndef __KEYBOARD_INPUT_EVENT_OBSERVER_H__
#define __KEYBOARD_INPUT_EVENT_OBSERVER_H__
#include <string>

class KeyboardInputEventObserver {
public:
    virtual ~KeyboardInputEventObserver() = default;

    // Called when a key is pressed
    virtual void on_key_pressed(const std::string& p_key) = 0;

    // Called when a key is released
    virtual void on_key_released(const std::string& p_key) = 0;
};

#endif
