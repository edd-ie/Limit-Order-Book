cmake_minimum_required(VERSION 3.31.6)

project(LimitOrderBook
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(LimitOrderBook main.cpp)

# Handle both GCC-style flags (clang++) and MSVC-style flags (clang-cl)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(MSVC)
        # Clang-cl optimization & safety flags
        target_compile_options(LimitOrderBook PRIVATE /W4 /WX /O2)
    else()
        # Standard Clang flags
        target_compile_options(LimitOrderBook PRIVATE -Wall -Wextra -O3)
    endif()
endif()

add_subdirectory(Structures)
target_link_libraries(LimitOrderBook
    PRIVATE
        Structures
)   
