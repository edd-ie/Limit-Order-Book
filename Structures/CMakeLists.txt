add_library(Structures STATIC)

target_sources(Structures
    PRIVATE
        Order.cpp
        PriceLevel.cpp
    
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            Order.hpp
            PriceLevel.hpp
)

# Apply warning and optimization flags to the library
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(MSVC)
        target_compile_options(Structures PRIVATE /W4 /WX /O2)
    else()
        target_compile_options(Structures PRIVATE -Wall -Wextra -Werror -O3)
    endif()
endif()