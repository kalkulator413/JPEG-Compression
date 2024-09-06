#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "image/BlockedImage.h"
#include <iostream>

static uint8_t QUALITY = 100u;

static void doBlockingTest(const char* filename)
{
    PPMImg ppm(filename);
    BlockedImage b(ppm);

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
                    CHECK(currY(r, c) == ppm.Yp(8u * i + r, 8u * j + c));
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
    doBlockingTest("./Data/Raw/blackbuck.ppm");
    doBlockingTest("./Data/Raw/first.ppm");
    doBlockingTest("./Data/Raw/sines.ppm");
    doBlockingTest("./Data/Raw/snail.ppm");
    doBlockingTest("./Data/Raw/sample1.ppm");
    doBlockingTest("./Data/Raw/sample2.ppm");
    doBlockingTest("./Data/Raw/sample3.ppm");
}

TEST_CASE("Reading Images", "[img][!benchmark]")
{
    BENCHMARK("Read PPM")
    {
        return PPMImg("./Data/Raw/sample1.ppm");
    };

    PPMImg ppm("./Data/Raw/sample1.ppm");
    BENCHMARK("MakingChunks")
    {
        return BlockedImage(ppm);
    };

    BlockedImage b24(ppm);

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
        b24.encode(QUALITY,"/dev/null");
    };
}

static void processPPMFile(std::string str)
{
    std::string oldName = "./Data/Raw/" + str + ".ppm";
    PPMImg ppm(oldName.c_str());
    BlockedImage b(ppm);
    b.applyDCT();
    b.quantize(QUALITY);
    std::string newName = "./Data/Processed/" + str + ".jpg";
    std::cout << "created " << newName << std::endl;
    b.encode(QUALITY, newName.c_str());
}

TEST_CASE("Writing files")
{
    processPPMFile("blackbuck");
    processPPMFile("first");
    processPPMFile("sines");
    processPPMFile("snail");
    processPPMFile("sample1");
    processPPMFile("sample2");
    processPPMFile("sample3");
};

