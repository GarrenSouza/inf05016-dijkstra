cmake_minimum_required(VERSION 3.16)

project(dijkstra-strikes-again)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(${PROJECT_NAME}_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/heap.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/graph.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
        )

add_executable(dijkstra ${${PROJECT_NAME}_SOURCES})

set(test_cases_sources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/test_case_generator.cpp"
        )

add_executable(test_cases ${test_cases_sources})

set_property(TARGET dijkstra PROPERTY CXX_STANDARD 11)
set_property(TARGET test_cases PROPERTY CXX_STANDARD 11)

target_include_directories(dijkstra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
