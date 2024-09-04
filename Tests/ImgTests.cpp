#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "image/BlockedImage.h"
#include <iostream>

static uint8_t QUALITY = 100u;

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

    // std::cout << "AFTER DCT:" << std::endl;
    // for (size_t r = 0; r < 8; ++r)
    // {
    //     for (size_t c = 0; c < 8; ++c)
    //     {
    //         std::cout << "(" << round(100* b.Y[0][r * 8 + c]) / 100.f << ", "
    //             << round(100* b.Cr[0][r * 8 + c]) / 100.f << ", "
    //             << round(100* b.Cb[0][r * 8 + c]) / 100.f << "), ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "----------" << std::endl;
    // std::cout << std::endl;


    b.quantize(QUALITY);
    // std::cout << "AFTER QUANTIZATION:" << std::endl;
    // for (size_t r = 0; r < 8; ++r)
    // {
    //     for (size_t c = 0; c < 8; ++c)
    //     {
    //         std::cout << "(" << b.qY[0][r * 8 + c] << ", "
    //             << b.qCr[0][r * 8 + c] << ", "
    //             << b.qCb[0][r * 8 + c] << "), ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "----------" << std::endl;
    // std::cout << std::endl;
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
        b24.quantize<true>(QUALITY);
    };

    BENCHMARK("Encode")
    {
        b24.encode(QUALITY,"./Data/Processed/sampl2.jpg");
    };
}

static void processBMPFile(std::string str)
{
    std::string oldName = "./Data/Raw/" + str + ".bmp";
    BMPImg bmp(oldName.c_str());
    BlockedImage b(bmp);
    b.applyDCT();
    b.quantize(100);
    std::string newName = "./Data/Processed/" + str + ".jpg";
    std::cout << "created " << newName << std::endl;
    b.encode(QUALITY, newName.c_str());
}

TEST_CASE("Writing files")
{
    processBMPFile("bmp_24");
    processBMPFile("greenland_grid_velo");
    processBMPFile("sample2");
    processBMPFile("snail");
};

TEST_CASE("BMP24")
{
    BMPImg bmp("./Data/Raw/bmp_24.bmp");

    CHECK(round(bmp.Yp(0, 0)) == 76 - 128);
    CHECK(round(bmp.Yp(1, 0)) == 76 - 128);
    CHECK(round(bmp.Yp(0, 6)) == 150 - 128);
    CHECK(round(bmp.Yp(1, 7)) == 150 - 128);

    BlockedImage b(bmp);
    b.applyDCT();
    Matrix<float> fullRed = b.Y[10 * b.blockedCols];

    std::cout << "AFTER DCT:" << std::endl;
    for (size_t r = 0; r < 8; ++r)
    {
        for (size_t c = 0; c < 8; ++c)
        {
            std::cout << "(" << round(100* b.Y[10 * b.blockedCols][r * 8 + c]) / 100.f << ", "
                << round(100* b.Cr[10 * b.blockedCols][r * 8 + c]) / 100.f << ", "
                << round(100* b.Cb[10 * b.blockedCols][r * 8 + c]) / 100.f << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << "----------" << std::endl;
    std::cout << std::endl;


    b.quantize(100);
    std::cout << "AFTER QUANTIZATION:" << std::endl;
    for (size_t r = 0; r < 8; ++r)
    {
        for (size_t c = 0; c < 8; ++c)
        {
            std::cout << "(" << b.qY[10 * b.blockedCols][r * 8 + c] << ", "
                << b.qCr[10 * b.blockedCols][r * 8 + c] << ", "
                << b.qCb[10 * b.blockedCols][r * 8 + c] << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << "----------" << std::endl;
    std::cout << std::endl;

    b.encode(QUALITY, "./Data/Processed/bmp_24.jpg");
};
