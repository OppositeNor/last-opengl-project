file(GLOB scene_grid_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(SCENE_GRID_LIB STATIC
    ${scene_grid_source}
)

set(libs ${libs}
    SCENE_GRID_LIB
    PARENT_SCOPE
)
