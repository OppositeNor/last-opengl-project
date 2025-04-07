#include "framebuffer/cube_depth_framebuffer.h"
#include "graphics.h"

CubeDepthFramebuffer::CubeDepthFramebuffer(std::size_t cube_width)
    : width(cube_width) {

    GL_CALL(glGenTextures, 1, &texture_id);
    GL_CALL(glGenFramebuffers, 1, &id);
    GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, texture_id);
    // set the texture parameters for the cube map
    for (unsigned int i = 0; i < 6; ++i) {
        GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                width, width, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, id);
    GL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_id, 0);

    // disable color buffer for better performance
    GL_CALL(glDrawBuffer, GL_NONE);
    GL_CALL(glReadBuffer, GL_NONE);

    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

CubeDepthFramebuffer::~CubeDepthFramebuffer() {
    GL_CALL(glDeleteFramebuffers, 1, &id);
    GL_CALL(glDeleteTextures, 1, &texture_id);
}

void CubeDepthFramebuffer::bind() const {
    GL_CALL(glViewport, 0, 0, width, width);
    GL_CALL(glClearColor, 0.0f, 0.0f, 0.0f, 1.0f);
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, id);
    GL_CALL(glEnable, GL_DEPTH_TEST);
    GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
}

void CubeDepthFramebuffer::bind_cube() const {
    GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, texture_id);
}

void CubeDepthFramebuffer::unbind() const {
    GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

