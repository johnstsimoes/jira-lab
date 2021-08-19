find_package(GTest CONFIG REQUIRED)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(${CMAKE_PROJECT_NAME}_tst
                main.cpp
                basic_tests.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}_tst
                      ${CMAKE_PROJECT_NAME}_lib
                      GTest)

add_test(NAME ${PROJECT_NAME}_test COMMAND ${CMAKE_PROJECT_NAME}_tst)
