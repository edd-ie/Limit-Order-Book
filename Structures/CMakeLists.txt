add_library(Structures)

target_sources(Structures
    PRIVATE
        Order.cpp
        PriceLevel.cpp
    
    PUBLIC
        FILE_SET HEADERS
        FILES
            Order.hpp
            PriceLevel.hpp
)