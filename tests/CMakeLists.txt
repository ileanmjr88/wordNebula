# ============================================================================
# Test Configuration
# ============================================================================

find_package(GTest REQUIRED)
include(GoogleTest)

# Enable testing
enable_testing()

# ============================================================================
# Test Sources
# ============================================================================

# TextBuffer Tests
add_executable(test_textbuffer
    test_TextBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/Model/TextBuffer.cpp
)

target_link_libraries(test_textbuffer
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        spdlog::spdlog
)

target_include_directories(test_textbuffer
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

gtest_discover_tests(test_textbuffer)

# GapBuffer Tests
add_executable(test_gapbuffer
    test_GapBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/Model/GapBuffer.cpp
)

target_link_libraries(test_gapbuffer
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        spdlog::spdlog
)

target_include_directories(test_gapbuffer
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

gtest_discover_tests(test_gapbuffer)

# ============================================================================
# Code Coverage Target
# ============================================================================
if(ENABLE_COVERAGE)
    find_program(LCOV_EXE NAMES "lcov")
    find_program(GENHTML_EXE NAMES "genhtml")

    if(LCOV_EXE AND GENHTML_EXE)
        add_custom_target(coverage
            COMMAND ${LCOV_EXE} --directory . --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LCOV_EXE} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_EXE} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
            COMMAND ${GENHTML_EXE} coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
        message(STATUS "Coverage target available: make coverage")
    endif()
endif()
