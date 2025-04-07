#ifndef __CUBE_FRAMEBUFFER_H__
#define __CUBE_FRAMEBUFFER_H__
#include "utils.hpp"

class CubeDepthFramebuffer {
public:
    CubeDepthFramebuffer(std::size_t cube_width);

    virtual ~CubeDepthFramebuffer();

    /**
     * @brief Bind the framebuffer.
     */
    void bind() const;

    /**
     * @brief Bind the cube.
     */
    void bind_cube() const;

    /**
     * @brief Unbind the framebuffer.
     */
    void unbind() const;

    // Draw the framebuffer
    void draw();

private:
    unsigned int id;
    unsigned int texture_id;
    unsigned int rbo_id;
    std::size_t width;
};
#endif
