file(GLOB TEST_SOURCES
  "./*.cpp"
  "./*.hpp")

add_subdirectory(rapidcheck)

add_executable(testapp ${TEST_SOURCES})
target_link_libraries(testapp src rapidcheck)
