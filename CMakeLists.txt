cmake_minimum_required(VERSION 3.11)

project(my_first_game)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED true)

include(FetchContent)
FetchContent_Declare(
    allegro5
    GIT_REPOSITORY https://github.com/liballeg/allegro5.git
    GIT_TAG master
)
FetchContent_MakeAvailable(allegro5)

# Specify where to build Allegro
set(ALLEGRO_BUILD_SHARED_LIBS ON CACHE BOOL "Build Allegro as shared libraries")

add_executable(my_first_game main.cpp)

target_link_libraries(my_first_game allegro allegro_primitives allegro_font)

# Specify where to copy the DLLs (e.g., in the same directory as your executable)
add_custom_command(TARGET my_first_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${allegro5_BINARY_DIR}/lib
        $<TARGET_FILE_DIR:my_first_game>
)