file(GLOB fb_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(FRAMEBUFFER STATIC
    ${fb_sources}
)

set(libs ${libs}
    FRAMEBUFFER
    PARENT_SCOPE
)
