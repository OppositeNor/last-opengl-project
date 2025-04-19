file(GLOB texture_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(TEXTURE STATIC
    ${texture_source}
)

set(libs ${libs}
    TEXTURE
    PARENT_SCOPE
)
