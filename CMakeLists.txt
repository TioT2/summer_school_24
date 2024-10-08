# CMakeList.txt : CMake project for ss_stk, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Declare project
project ("example")

# Create source list
file(GLOB_RECURSE example_src
    src/*.cpp
    src/*.h
)

add_executable (${PROJECT_NAME} ${example_src})

# Library customization example
# set(STK_OVERRIDE_CONFIG          GLOBAL)
# set(STK_ENABLE_CORRUPTION_CHECKS GLOBAL)

# Add library
add_subdirectory(library)

target_link_libraries(${PROJECT_NAME} ss_stk)
# Add source to the project's executable.

# CMakeLists.txt file end
