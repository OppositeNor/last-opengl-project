file(GLOB application_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(APPLICATION_LIB
    ${application_src}
)

set(libs ${libs}
    APPLICATION_LIB
    PARENT_SCOPE
)
