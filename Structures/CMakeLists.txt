add_library(Structures STATIC)

target_sources(Structures
    PRIVATE
        Order.cpp
        PriceLevel.cpp
        Book.cpp
        
    
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            Order.hpp
            PriceLevel.hpp
            Report.hpp
            Book.hpp
            Units.hpp
)

# Apply warnings to the library
if(MSVC)
    target_compile_options(Structures PRIVATE /W4 /WX)
else()
    target_compile_options(Structures PRIVATE -Wall -Wextra -Werror)
endif()
