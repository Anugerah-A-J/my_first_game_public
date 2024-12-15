cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
# project(my_first_game LANGUAGES CXX)
project(my_first_game)
include(FetchContent)

FetchContent_Declare(
    allegro5
    GIT_REPOSITORY https://github.com/liballeg/allegro5.git
    GIT_TAG master
    # GIT_TAG 5.2.8
)
FetchContent_MakeAvailable(allegro5)
# FetchContent_GetProperties(allegro5)
if(NOT allegro5_POPULATED)
  FetchContent_Populate(allegro5)
	if (MSVC)
		set(SHARED ON)
	else()
		set(SHARED OFF)
	endif()
	set(WANT_TESTS OFF)
	set(WANT_EXAMPLES OFF)
	set(WANT_DEMO OFF)
  add_subdirectory(${allegro5_SOURCE_DIR} ${allegro5_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Specify where to build Allegro
set(ALLEGRO_BUILD_SHARED_LIBS ON CACHE BOOL "Build Allegro as shared libraries")

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings and debug
    add_compile_options(-Wall -Wextra -Wpedantic -g)
endif()

add_executable(my_first_game main.cpp)
target_include_directories(my_first_game PUBLIC ${allegro5_SOURCE_DIR}/include)
target_include_directories(my_first_game PUBLIC ${allegro5_BINARY_DIR}/include)
target_link_libraries(my_first_game LINK_PUBLIC allegro allegro_primitives allegro_font)

# These include files are typically copied into the correct places via allegro's install
# target, but we do it manually.
file(COPY ${allegro5_SOURCE_DIR}/addons/font/allegro5/allegro_font.h
	DESTINATION ${allegro5_SOURCE_DIR}/include/allegro5
)
file(COPY ${allegro5_SOURCE_DIR}/addons/primitives/allegro5/allegro_primitives.h
	DESTINATION ${allegro5_SOURCE_DIR}/include/allegro5
)

# Specify where to copy the DLLs (e.g., in the same directory as your executable)
add_custom_command(TARGET my_first_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${allegro5_BINARY_DIR}/lib
        $<TARGET_FILE_DIR:my_first_game>
)
