#include <catch2/catch_test_macros.hpp>
#include "math/Matrix.h"
#include <cstddef>

template<typename T>
void checkOnes(size_t nRows, size_t nCols)
{
    Matrix<T> m(nRows, nCols);
    m.setOnes();
    for (size_t i = 0; i < nRows; ++i)
    {
        for (size_t j = 0; j < nCols; ++j)
        {
            CHECK(m(i, j) == T(1));
        }
    }

    Matrix<T, 32> m1(nRows, nCols);
    m1.setOnes();
    for (size_t i = 0; i < nRows; ++i)
    {
        for (size_t j = 0; j < nCols; ++j)
        {
            CHECK(m1(i, j) == T(1));
        }
    }
}

TEST_CASE( "SetOnes", "[matrix]" )
{
    for (size_t r = 3; r < 6; ++r)
    {
        for (size_t c = 3; c < 6; ++c)
        {
            checkOnes<float>(r, c);
            checkOnes<double>(r, c);
            checkOnes<uint8_t>(r, c);
            checkOnes<uint16_t>(r, c);
            checkOnes<uint32_t>(r, c);
            checkOnes<int8_t>(r, c);
            checkOnes<int16_t>(r, c);
            checkOnes<int32_t>(r, c);
        }
    }
}

template<typename T>
void checkZeros(size_t nRows, size_t nCols)
{
    Matrix<T> m(nRows, nCols);
    m.setZeros();
    for (size_t i = 0; i < nRows; ++i)
    {
        for (size_t j = 0; j < nCols; ++j)
        {
            CHECK(m(i, j) == T(0));
        }
    }

    Matrix<T, 32> m1(nRows, nCols);
    m1.setZeros();
    for (size_t i = 0; i < nRows; ++i)
    {
        for (size_t j = 0; j < nCols; ++j)
        {
            CHECK(m1(i, j) == T(0));
        }
    }
}

TEST_CASE( "SetZeros", "[matrix]" )
{
    for (size_t r = 3; r < 6; ++r)
    {
        for (size_t c = 3; c < 6; ++c)
        {
            checkZeros<float>(r, c);
            checkZeros<double>(r, c);
            checkZeros<uint8_t>(r, c);
            checkZeros<uint16_t>(r, c);
            checkZeros<uint32_t>(r, c);
            checkZeros<int8_t>(r, c);
            checkZeros<int16_t>(r, c);
            checkZeros<int32_t>(r, c);
        }
    }
}
