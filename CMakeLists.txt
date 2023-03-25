cmake_minimum_required(VERSION 3.16)

project(heap)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(${PROJECT_NAME}_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/heap.cpp"
        )

add_library(heap ${${PROJECT_NAME}_SOURCES})

set_property(TARGET heap PROPERTY CXX_STANDARD 20)

target_include_directories(heap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
