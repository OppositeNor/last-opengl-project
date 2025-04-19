file(GLOB system_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(SYSTEM_LIB STATIC
    ${system_source}
)

set(libs ${libs}
    SYSTEM_LIB
    PARENT_SCOPE
)

