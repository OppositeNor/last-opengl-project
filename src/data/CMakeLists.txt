file(GLOB data_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(DATA_LIB STATIC
    ${data_sources}
)

set(libs ${libs}
    DATA_LIB
    PARENT_SCOPE
)
