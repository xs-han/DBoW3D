cmake_minimum_required(VERSION 2.8)

project(VirtualView)
# The version number.
set (TESTS_VERSION_MAJOR 1)
set (TESTS_VERSION_MINOR 0)

find_package(OpenCV 3.2.0 REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/include 
    ${OpenCV_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/DBoW3)

# DBoW3 Compilation
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
aux_source_directory(${PROJECT_SOURCE_DIR}/DBoW3 DBoW3_SRCS)
add_compile_options(-std=c++11)
add_library(DBoW3 SHARED ${DBoW3_SRCS})
target_link_libraries(DBoW3 ${OpenCV_LIBS})

# VirtualView Compilation
aux_source_directory(${PROJECT_SOURCE_DIR}/src VIRTUALVIEW_SRCS)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_compile_options(-std=c++11)
add_executable(main ${VIRTUALVIEW_SRCS})
target_link_libraries(main DBoW3 ${OpenCV_LIBS})

FILE(GLOB_RECURSE TESTS_DIR_INCLUDE "${PROJECT_SOURCE_DIR}/include/*.h")
add_custom_target(headers SOURCES ${TESTS_DIR_INCLUDE})
