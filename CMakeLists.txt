cmake_minimum_required(VERSION 3.5)
 
project(JPEG-Compression)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14")
 
add_subdirectory(Lib)
add_subdirectory(App)
add_subdirectory(Tests)
