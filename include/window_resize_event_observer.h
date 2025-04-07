#ifndef __WINDOW_RESIZE_EVENT_OBSERVER_H__
#define __WINDOW_RESIZE_EVENT_OBSERVER_H__

class WindowResizeEventObserver {
public:
    virtual void on_resize(unsigned int p_updated_x, unsigned int p_updated_y) = 0;
};

#endif
