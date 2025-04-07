#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__
#include <array>

struct TextureConfigurations {
    bool generate_mipmap = false;
    bool scale_nearest_min = true;
    bool scale_nearest_mag = true;
    bool srgb = false;
    enum WrapMode {
        REPEAT,
        MORRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };
    WrapMode wrap_mode_s = WrapMode::REPEAT;
    WrapMode wrap_mode_t = WrapMode::REPEAT;
    WrapMode wrap_mode_r = WrapMode::REPEAT; // Only for 3D
    enum TextureFace {
        FRONT,BACK,
        TOP,BOTTOM,
        LEFT,RIGHT
    };
    enum RotateMode {
        NONE, CW, CCW, ONE_EIGHTY
    };
    std::array<RotateMode, 6> rotate_config;
};

#endif
