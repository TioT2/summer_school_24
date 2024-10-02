# CMakeList.txt : CMake project for ss_stk, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Declare project
project ("ss_stk")

# Create source list
file(GLOB_RECURSE ss_stk_src # TODO: ? # ???
  src/*.cpp
  src/*.h
)

# Add source to this project's executable.
add_executable (ss_stk  ${ss_stk_src})

set_property(TARGET ss_stk PROPERTY CXX_STANDARD 20)

# TODO: sh*tcode
add_compile_definitions(TARGET ss_stk _DEBUG)

# CMakeLists.txt file end
