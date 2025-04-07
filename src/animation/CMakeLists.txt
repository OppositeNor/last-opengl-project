file(GLOB animation_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(ANIMATION_LIB
    ${animation_sources}
)

set(libs ${libs}
    ANIMATION_LIB
    PARENT_SCOPE
)
