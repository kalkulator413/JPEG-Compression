#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "image/BlockedImage.h"

static void doBlockingTest(const char* filename)
{
    BMPImg bmp(filename);
    BlockedImage b(bmp);

    for (size_t i = 0; i < b.blockedRows; ++i)
    {
        for (size_t j = 0; j < b.blockedCols; ++j)
        {
            Matrix<float> currY = b.Y[i * b.blockedCols + j];
            // check each element in block
            for (size_t r = 0; r < 8; ++r)
            {
                for (size_t c = 0; c < 8; ++c)
                {
                    CHECK(currY(r, c) == bmp.Yp(8u * i + r, 8u * j + c));
                }
            }
        }
    }
}

TEST_CASE("BlockSetup", "[img]")
{
    doBlockingTest("./Data/Raw/bmp_24.bmp");
    doBlockingTest("./Data/Raw/greenland_grid_velo.bmp");
    doBlockingTest("./Data/Raw/sample2.bmp");
    doBlockingTest("./Data/Raw/snail.bmp");
}

TEST_CASE("Reading Images", "[img][!benchmark]")
{
    BENCHMARK("BMP24")
    {
        return BMPImg("./Data/Raw/bmp_24.bmp");
    };

    BMPImg bmp24("./Data/Raw/bmp_24.bmp");
    BENCHMARK("MakingChunks")
    {
        return BlockedImage(bmp24);
    };
}
