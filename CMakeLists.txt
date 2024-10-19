cmake_minimum_required(VERSION 3.10)

project(Emulator C) # Specify the language here

# Set the C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Define the paths to the raylib library and headers + net_utils
set(RAYLIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/raylib)



# Find all source files
file(GLOB_RECURSE SRC "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.c")



add_executable(main ${SRC} )


# target_compile_options(main PRIVATE -Wall -Wno-ignore-pragmas -Wno-type-limits -Wno-implicit-function-declaration -std=c2x)


target_include_directories(main PUBLIC ${RAYLIB_PATH}/include )

# Automatically find all .a files in the RAYLIB_PATH/lib directory and link them
file(GLOB RAYLIB_LIBRARY_PATH "${RAYLIB_PATH}/lib/*.a")

target_link_libraries(main ${RAYLIB_LIBRARY_PATH} pthread m)

if(WIN32)
    target_link_libraries(main opengl32 gdi32 winmm)
endif()


