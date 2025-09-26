file(GLOB voxel_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(VOXEL STATIC
    ${voxel_source}
)

set(libs ${libs}
    VOXEL
    PARENT_SCOPE
)

