cmake_minimum_required(VERSION 3.23)

project(dijkstra-strikes-again)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(${PROJECT_NAME}_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/heap.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
        )

add_executable(test ${${PROJECT_NAME}_SOURCES})

set_property(TARGET test PROPERTY CXX_STANDARD 11)

target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
