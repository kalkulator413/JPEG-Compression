#include <catch2/catch_test_macros.hpp>
#include "image/BMP.h"
#include "math/Matrix.h"

TEST_CASE("Setup", "[img]")
{
    // Magick::Image image;
    // image.read("./Data/Raw/dots.bmp"); 
    // image.display();
    
    BMPImg bmp("./Data/Raw/bmp_24.bmp");
    // bmp.display();

    // Magick::Image image()
    // Matrix<uint8_t, 0> red8x8Chunk(8, 8);
    
        

}