add_executable(unit_tests
    OrderTest.cpp
    PriceLevelTest.cpp
)

target_link_libraries(unit_tests
    PRIVATE
        Structures
        gtest_main
)

# Register with CTest
gtest_discover_tests(unit_tests)