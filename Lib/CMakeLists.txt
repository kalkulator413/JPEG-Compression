add_library(Lib 
    src/Image.cpp
)

message("Lib current source dir = ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
