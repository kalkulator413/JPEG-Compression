add_library(Lib 
    src/BMP.cpp
    src/BlockedImage.cpp
)

message("Lib current source dir = ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
