cmake_minimum_required(VERSION 3.31.6)

project(LimitOrderBook
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Suppress Clang warning on GoogleTest targets
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    foreach(target gtest gtest_main gmock gmock_main)
        if(TARGET ${target})
            target_compile_options(${target} PRIVATE -Wno-character-conversion)
        endif()
    endforeach()
endif()

# Enable CTest integration
enable_testing()
include(GoogleTest)

# Add source libraries and tests
add_subdirectory(Structures)

# Production Executable
add_executable(LimitOrderBook main.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(MSVC)
        target_compile_options(LimitOrderBook PRIVATE /W4 /WX /O2)
    else()
        target_compile_options(LimitOrderBook PRIVATE -Wall -Wextra -Werror -O3)
    endif()
endif()

target_link_libraries(LimitOrderBook PRIVATE Structures)

# Add Test Suite
add_subdirectory(tests)