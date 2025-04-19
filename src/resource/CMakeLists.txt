file(GLOB resource_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(RESOURCE_LIB STATIC STATIC
    ${resource_src}
)

set(libs ${libs}
    RESOURCE_LIB
    PARENT_SCOPE
)
