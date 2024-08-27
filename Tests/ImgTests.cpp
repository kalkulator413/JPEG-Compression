#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "image/BMP.h"

TEST_CASE("Setup", "[img]")
{
    // Magick::Image image;
    // image.read("./Data/Raw/dots.bmp"); 
    // image.display();
    
    BMPImg bmp1("./Data/Raw/bmp_24.bmp");
    BMPImg bmp2("./Data/Raw/greenland_grid_velo.bmp");
    BMPImg bmp3("./Data/Raw/sample2.bmp");
    BMPImg bmp4("./Data/Raw/snail.bmp");
    // bmp.display();

    // Magick::Image image()
    // Matrix<uint8_t, 0> red8x8Chunk(8, 8);
}

TEST_CASE("Reading Images", "[img][!benchmark]")
{
    BENCHMARK("BMP24") {
        return BMPImg("./Data/Raw/bmp_24.bmp");
    };
}
