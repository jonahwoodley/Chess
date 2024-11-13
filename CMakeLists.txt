cmake_minimum_required(VERSION 3.20)
project(chess VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

include_directories(src)
include_directories(external/imgui-1.91.4)

file(GLOB SOURCES "src/*.cpp")
file(GLOB IMGUI_SOURCES "external/imgui-1.91.4/*.cpp")

add_executable(chess ${SOURCES} ${IMGUI_SOURCES})

target_link_libraries(chess SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)

# copy resources to build directory.
add_custom_command(TARGET chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:chess>/resources
)
