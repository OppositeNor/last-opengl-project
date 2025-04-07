#ifndef __RESOURCE_BASE_H__
#define __RESOURCE_BASE_H__

class ResourceBase {
public:

    virtual void load() = 0;

    // True if the resource can only be loaded on the main thread. This usually applies to OpenGL specific resources.
    bool get_is_main_only() const { return is_main_load_only; }

protected:
    void set_is_main_only(bool p_is_main_load_only) { is_main_load_only = p_is_main_load_only; }

private:
    bool is_main_load_only = false;
};

template <typename T>
struct get_res_type;

#endif
