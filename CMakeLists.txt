cmake_minimum_required(VERSION 3.18)

project(FinalProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

option(USE_LOCAL_DEPS "Use the submodule dependencies to build." OFF)
option(USE_SDL "Use SDL" ON)

message("Build type: ${CMAKE_BUILD_TYPE}")

if (NOT (CMAKE_BUILD_TYPE STREQUAL "Release"))
    if (NOT USE_LOCAL_DEPS AND NOT WIN32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Wextra")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    endif()

    if (NOT USE_SDL)    # For some reason this doesn't work with SDL
        set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
        set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${ASAN_FLAGS}")
    endif()

    # valgrind test:
    # valgrind --leak-check=full --track-origins=yes ./build/FinalProject
endif()

# default include dirs
set(include_dirs
    include
    dependencies/glad/include
    dependencies/imgui
    dependencies/stb
    dependencies/MMDFormats
)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
    set(include_dirs ${include_dirs} ${PROJECT_SOURCE_DIR}/build/dependencies/assimp/include)
endif()

set(libs "")

if (USE_SDL)
    set(include_dirs ${include_dirs} dependencies/SDL2/include) # imgui needs this
    add_definitions(-DUSE_SDL)
    set(libs ${libs} SDL2::SDL2)
else()
    set(libs ${libs} glfw)
endif()

# add local deps as subdirectories
if (USE_LOCAL_DEPS OR WIN32)
    # add assimp subdirectory
    set(ASSIMP_BUILD_TESTS OFF)
    set(ASSIMP_INSTALL OFF)
    add_subdirectory(dependencies/assimp)


    # add glm subdirectory
    add_subdirectory(dependencies/glm)

    # add tomlplusplus subdirectory
    add_subdirectory(dependencies/tomlplusplus)

    if (USE_SDL)
        # add SDL2 subdirectory
        set(SDL_TEST OFF)
        set(SDL_STATIC ON)
        set(SDL_SHARED OFF)
        add_subdirectory(dependencies/SDL2)
    else()
        # add GLFW subdirectory
        set(GLFW_BUILD_EXAMPLES OFF)
        set(GLFW_BUILD_TESTS OFF)
        add_subdirectory(dependencies/glfw)
        set(include_dirs ${include_dirs} dependencies/glfw/include) # imgui needs this
    endif()

    set(include_dirs ${include_dirs}
        dependencies/assimp/include
        dependencies/glm
        dependencies/tomlplusplus/include
    )
    add_definitions(-DUSE_LOCAL_DEPS)
endif()


file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

# ImGui sources
set(sources
    ${sources}
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

if(USE_SDL)
    set(sources
        ${sources}
        ${PROJECT_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_sdl2.cpp
    )
else()
    set(sources
        ${sources}
        ${PROJECT_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_glfw.cpp
    )
endif()

# MMDFormats sources
if (NOT WIN32)
set(sources
    ${sources}
    ${PROJECT_SOURCE_DIR}/dependencies/MMDFormats/MikuMikuFormats/Pmd.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/MMDFormats/MikuMikuFormats/Pmx.cpp
)
endif()

include_directories(${PROJECT_NAME}
    ${include_dirs}
)

if(UNIX)
    find_package(ICU COMPONENTS uc i18n REQUIRED)
    set(libs
        ${libs}
        OpenGL
        m
        ICU::uc
        ICU::i18n
    )
endif()

# project subdirectories
add_subdirectory(src/system)
add_subdirectory(src/application)
add_subdirectory(src/shader)
add_subdirectory(src/framebuffer)
add_subdirectory(src/resource)
add_subdirectory(src/scene_node)
add_subdirectory(src/data)
add_subdirectory(src/animation)
add_subdirectory(src/material)
add_subdirectory(src/scene_grid)
add_subdirectory(src/texture)

add_executable(${PROJECT_NAME}
    ${sources}
    ${PROJECT_SOURCE_DIR}/dependencies/glad/src/glad.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glm
    assimp
    tomlplusplus::tomlplusplus
    ${libs}
)

file(GLOB resources ${RESOURCE_DIR}/*)
file(COPY ${resources} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
