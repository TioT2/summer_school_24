# CMakeList.txt : CMake project for ss_stk, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Declare project
project ("ss_stk")

# Create source list
file(GLOB_RECURSE ss_stk_src
    src/*.cpp
    src/*.h
)

set(GLOB_RECURSE ss_stk_include
    include/*.h
)

# Add source to the project's executable.
add_library (${PROJECT_NAME} ${ss_stk_src} ${ss_stk_include})

target_include_directories(${PROJECT_NAME} PUBLIC include)

# Set C++ standard
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Add _DEBUG and _NDEBUG definitions
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      $<$<CONFIG:Debug>:_DEBUG>
      $<$<CONFIG:RelWithDebInfo>:_DEBUG>
      $<$<CONFIG:Release>:_NDEBUG>
      $<$<CONFIG:MinSizeRel>:_NDEBUG>
)

# Add platform-identifying definitions
if (WIN32)
    add_compile_definitions(TARGET ${PROJECT_NAME} STK_PLATOFRM_WIN32)
elseif (UNIX)
    add_compile_definitions(TARGET ${PROJECT_NAME} STK_PLATOFRM_UNIX)
else ()
    message("Failed to identify target platform. Some validation checks are disabled.")
    add_compile_definitions(TARGET ${PROJECT_NAME} STK_PLATFORM_UNDEFINED)
endif ()

# CMakeLists.txt file end
