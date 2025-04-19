file(GLOB shader_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(SHADER_LIB STATIC
    ${shader_source}
)

set(libs ${libs}
    SHADER_LIB
    PARENT_SCOPE
)

