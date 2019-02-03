add_definitions(-DLOGGING_ENABLED)

add_executable(test_cpu test_cpu.cpp)
target_link_libraries(test_cpu nes ${GTEST_BOTH_LIBRARIES})
add_test(NAME test_cpu COMMAND test_cpu)
