cmake_minimum_required(VERSION 3.5)
project(money VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 11)
include(CTest)
enable_testing()
find_package(Threads REQUIRED)
add_executable(money money.cpp)

target_link_libraries(money
        gtest
        gtest_main
        ${CMAKE_THREAD_LIBS_INIT})
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
