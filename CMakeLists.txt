
cmake_minimum_required(VERSION 3.26)

project(smart_list VERSION 1.0.0)

# load conan

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	include (${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup (#[[TARGETS]])
endif ()

#
# LIB
#

add_library(smart_list INTERFACE)

target_include_directories(smart_list INTERFACE src/)

#
# TEST
#

find_package(GTest REQUIRED)

add_executable(smart_list_unit_tests src/test/test.cpp)

target_link_libraries(smart_list_unit_tests GTest::gtest_main smart_list)
