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

# target_include_directories(${PROJECT_NAME} PRIVATE library/include)
add_subdirectory(library)
target_link_libraries(${PROJECT_NAME} ss_stk)
# Add source to the project's executable.

# CMakeLists.txt file end
