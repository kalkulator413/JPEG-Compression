cmake_minimum_required(VERSION 3.5)
 
project(JPEG-Compression)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")
 
add_subdirectory(Lib)
add_subdirectory(App)
