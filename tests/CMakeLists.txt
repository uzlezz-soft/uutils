enable_testing()

add_executable(uutils_tests
    test_data_processing.cpp
)

target_link_libraries(uutils_tests
    PRIVATE
        uutils
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(uutils_tests)

target_include_directories(uutils_tests PRIVATE ../uutils/include)