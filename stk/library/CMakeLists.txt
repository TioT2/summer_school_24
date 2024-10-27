# CMakeList.txt : CMake project for ss_stk, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Declare project
project ("ss_stk")

# Create source list
file(GLOB_RECURSE ss_stk_src
    src/*.cpp
)

set(GLOB_RECURSE ss_stk_include
    include/*.h
)

# Add source to the project's executable.
add_library (${PROJECT_NAME} ${ss_stk_src} ${ss_stk_include})

# Include 'include' directory
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
    target_compile_definitions(${PROJECT_NAME} PUBLIC STK_PLATFORM_WIN32)
elseif (UNIX)
target_compile_definitions(${PROJECT_NAME} PUBLIC STK_PLATFORM_UNIX)
else ()
    message("Failed to identify target platform. Some validation checks are disabled.")
    target_compile_definitions(${PROJECT_NAME} PUBLIC STK_PLATFORM_UNDEFINED)
endif ()

if (DEFINED STK_OVERRIDE_CONFIG)
    target_compile_definitions(${PROJECT_NAME} PUBLIC STK_OVERRIDE_CONFIG)

    # repeating code...
    macro(STK_HANDLE_CONFIG_ELEMENT ENTRY)
        if (DEFINED ${ENTRY})
            target_compile_definitions(${PROJECT_NAME} PUBLIC ${ENTRY})
        endif()
    endmacro()

    STK_HANDLE_CONFIG_ELEMENT(STK_ENABLE_CORRUPTION_CHECKS)
    STK_HANDLE_CONFIG_ELEMENT(STK_ENABLE_DEBUG_INFO       )
    STK_HANDLE_CONFIG_ELEMENT(STK_ENABLE_CANARIES         )
    STK_HANDLE_CONFIG_ELEMENT(STK_ENABLE_HASHING          )
    STK_HANDLE_CONFIG_ELEMENT(STK_ENABLE_POISONING        )
    STK_HANDLE_CONFIG_ELEMENT(STK_ENABLE_HANDLE_ENCRYPTION)
endif()

# CMakeLists.txt file end
