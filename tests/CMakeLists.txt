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
add_executable(
  test_textbuffer test_TextBuffer.cpp
                  ${CMAKE_SOURCE_DIR}/src/Model/TextBuffer.cpp
)

target_link_libraries(
  test_textbuffer PRIVATE GTest::gtest GTest::gtest_main spdlog::spdlog
)

target_include_directories(test_textbuffer PRIVATE ${CMAKE_SOURCE_DIR}/include)

gtest_discover_tests(test_textbuffer)

# WNebulaPresenter Tests
add_executable(
  test_presenter
  test_WNebulaPresenter.cpp
  ${CMAKE_SOURCE_DIR}/src/Presenter/WNebulaPresenter.cpp
  ${CMAKE_SOURCE_DIR}/src/Model/WNebulaModel.cpp
  ${CMAKE_SOURCE_DIR}/src/Model/GapBuffer.cpp
  ${CMAKE_SOURCE_DIR}/src/View/WNebulaView.cpp
)

target_link_libraries(
  test_presenter PRIVATE GTest::gtest GTest::gtest_main spdlog::spdlog ncurses
)

target_include_directories(test_presenter PRIVATE ${CMAKE_SOURCE_DIR}/include)

gtest_discover_tests(test_presenter)

# FtxuiView Tests
add_executable(
  test_ftxuiview test_FtxuiView.cpp ${CMAKE_SOURCE_DIR}/src/View/FtxuiView.cpp
)

target_link_libraries(
  test_ftxuiview PRIVATE GTest::gtest GTest::gtest_main ftxui::screen
                         ftxui::dom ftxui::component
)

target_include_directories(test_ftxuiview PRIVATE ${CMAKE_SOURCE_DIR}/include)

gtest_discover_tests(test_ftxuiview)

# GapBuffer Tests (Optional - In Development) GapBuffer is currently being
# redesigned - enable only for active development
option(BUILD_GAPBUFFER_TESTS "Build GapBuffer tests (currently in development)"
       OFF
)

if(BUILD_GAPBUFFER_TESTS)
  message(STATUS "Building GapBuffer tests (experimental)")

  add_executable(
    test_gapbuffer test_GapBuffer.cpp
                   ${CMAKE_SOURCE_DIR}/src/Model/GapBuffer.cpp
  )

  target_link_libraries(
    test_gapbuffer PRIVATE GTest::gtest GTest::gtest_main spdlog::spdlog
  )

  target_include_directories(test_gapbuffer PRIVATE ${CMAKE_SOURCE_DIR}/include)

  gtest_discover_tests(test_gapbuffer)
else()
  message(
    STATUS "Skipping GapBuffer tests (enable with -DBUILD_GAPBUFFER_TESTS=ON)"
  )
endif()

# ============================================================================
# Code Coverage Target
# ============================================================================
if(ENABLE_COVERAGE)
  find_program(LCOV_EXE NAMES "lcov")
  find_program(GENHTML_EXE NAMES "genhtml")

  if(LCOV_EXE AND GENHTML_EXE)
    add_custom_target(
      coverage
      DEPENDS test_textbuffer test_presenter test_ftxuiview
      COMMAND ${LCOV_EXE} --directory . --zerocounters
      COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
      COMMAND ${LCOV_EXE} --directory . --capture --output-file coverage.info
      COMMAND ${LCOV_EXE} --remove coverage.info '/usr/*' '*/tests/*'
              --output-file coverage.info
      COMMAND ${GENHTML_EXE} coverage.info --output-directory
              ${CMAKE_BINARY_DIR}/coverage
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating code coverage report"
    )
    message(STATUS "Coverage target available: make coverage")
  endif()
endif()
