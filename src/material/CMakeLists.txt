file(GLOB material_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(MATERIAL_LIB STATIC
    ${material_src}
)

set(libs ${libs}
    MATERIAL_LIB
    PARENT_SCOPE
)
