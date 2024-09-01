#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "image/BlockedImage.h"
#include <iostream>

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

    b.applyDCT();

    std::cout << "AFTER DCT:" << std::endl;
    for (size_t r = 0; r < 8; ++r)
    {
        for (size_t c = 0; c < 8; ++c)
        {
            std::cout << "(" << round(100* b.Y[0][r * 8 + c]) / 100.f << ", "
                << round(100* b.Cr[0][r * 8 + c]) / 100.f << ", "
                << round(100* b.Cb[0][r * 8 + c]) / 100.f << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << "----------" << std::endl;
    std::cout << std::endl;


    b.quantize();
    std::cout << "AFTER QUANTIZATION:" << std::endl;
    for (size_t r = 0; r < 8; ++r)
    {
        for (size_t c = 0; c < 8; ++c)
        {
            std::cout << "(" << b.qY[0][r * 8 + c] << ", "
                << b.qCr[0][r * 8 + c] << ", "
                << b.qCb[0][r * 8 + c] << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << "----------" << std::endl;
    std::cout << std::endl;
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
        return BMPImg("./Data/Raw/sample2.bmp");
    };

    BMPImg bmp24("./Data/Raw/sample2.bmp");
    BENCHMARK("MakingChunks")
    {
        return BlockedImage(bmp24);
    };

    BlockedImage b24(bmp24);

    BENCHMARK("DCT")
    {
        b24.applyDCT();
    };

    BENCHMARK("Quantize")
    {
        b24.quantize<true>();
    };
}
