add_library(Lib 
    src/PPM.cpp
    src/BlockedImage.cpp
    src/Bitstream.cpp
)

message("Lib current source dir = ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(ImageMagick REQUIRED COMPONENTS Magick++)
target_link_libraries(Lib PRIVATE ImageMagick::Magick++)
target_include_directories(Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)