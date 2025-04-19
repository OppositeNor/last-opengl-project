file(GLOB sn_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(SCENE_NODE STATIC
    ${sn_sources}
)

set(libs ${libs}
    SCENE_NODE
    PARENT_SCOPE
)
