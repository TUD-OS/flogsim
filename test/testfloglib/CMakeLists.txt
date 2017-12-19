file(GLOB TEST_SRC *.cpp)
add_executable(unit_test ${TEST_SRC})
target_link_libraries(unit_test libgtest libgmock flog stdc++fs)

add_test(unit_test unit_test)
