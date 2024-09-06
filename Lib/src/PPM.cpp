#include <fstream>
#include <iostream>
#include <cassert>
#include <image/PPM.h>
#include <Magick++.h>

PPMImg::PPMImg(const char * filename)
    : Yp(0, 0), Cb(0, 0), Cr(0, 0)
{
    // using namespace std;

    // ifstream inf(filename);
    // string magicNumber;
    // size_t cols;
    // size_t rows;
    // uint maxColorValue;

    // if(!inf.is_open())
    //     cerr << "Unable to open file: " << filename << std::endl;

    // inf >> magicNumber >> cols >> rows >> maxColorValue;

    // if (magicNumber != "P6")
    //     cerr << "Invalid PPM file format." << endl;

    // if (maxColorValue != 255u)
    //     cerr << "I have no idea what to do." << endl;

    // fullRows = std::ceil(rows / 8.0f) * 8u;
    // fullCols = std::ceil(cols / 8.0f) * 8u;

    // Yp = Matrix<float>(fullRows, fullCols);
    // Cb = Matrix<float>(fullRows, fullCols);
    // Cr = Matrix<float>(fullRows, fullCols);

    // // todo: use SIMD for this
    // for (size_t r = 0; r < rows; ++r)
    // {
    //     for (size_t c = 0; c < cols; ++c)
    //     {
    //         uint8_t rd;
    //         uint8_t gr;
    //         uint8_t bl;

    //         inf >> skipws >> rd;
    //         inf >> skipws >> gr;
    //         inf >> skipws >> bl;

    //         float y = 0.299f * rd + 0.587f * gr + 0.114f * bl - 128;
    //         float cb = -0.1687f * rd - 0.33126f * gr + 0.5f * bl;
    //         float cr = 0.5f * rd - 0.41869f * gr - 0.0813f * bl;

    //         Yp(r, c) = y;
    //         Cb(r, c) = cb;
    //         Cr(r, c) = cr;
    //     }
    // }

    // // padding (vertically)
    // for (size_t r = rows; r < fullRows; ++r)
    // {
    //     for (size_t c = 0; c < cols; ++c)
    //     {
    //         Yp(r, c) = Yp(rows - 1u, c);
    //         Cr(r, c) = Cr(rows - 1u, c);
    //         Cb(r, c) = Cb(rows - 1u, c);
    //     }
    // }

    // // padding (horizontally)
    // for (size_t c = cols; c < fullCols; ++c)
    // {
    //     for (size_t r = 0; r < fullRows; ++r)
    //     {
    //         Yp(r, c) = Yp(r, cols - 1u);
    //         Cr(r, c) = Cr(r, cols - 1u);
    //         Cb(r, c) = Cb(r, cols - 1u);
    //     }
    // }

    Magick::Image img(filename);
    img.depth(8);
    auto pack = img.getPixels(0, 0, img.columns(), img.rows());

    fullRows = std::ceil(img.rows() / 8.0f) * 8u;
    fullCols = std::ceil(img.columns() / 8.0f) * 8u;

    Yp = Matrix<float>(fullRows, fullCols);
    Cb = Matrix<float>(fullRows, fullCols);
    Cr = Matrix<float>(fullRows, fullCols);

    for (size_t r = 0; r < img.rows(); ++r)
    {
        for (size_t c = 0; c < img.columns(); ++c)
        {
            auto x = pack[(r) * img.columns() + c];
            uint8_t rd = uint8_t(x.red);
            uint8_t gr = uint8_t(x.green);
            uint8_t bl = uint8_t(x.blue);
            float y = 0.299f * rd + 0.587f * gr + 0.114f * bl - 128;
            float cb = -0.1687f * rd - 0.33126f * gr + 0.5f * bl;
            float cr = 0.5f * rd - 0.41869f * gr - 0.0813f * bl;

            Yp(r, c) = y;
            Cb(r, c) = cb;
            Cr(r, c) = cr;
        }
    }

    // padding (vertically)
    for (size_t r = img.rows(); r < fullRows; ++r)
    {
        for (size_t c = 0; c < img.columns(); ++c)
        {
            Yp(r, c) = Yp(img.rows() - 1u, c);
            Cr(r, c) = Cr(img.rows() - 1u, c);
            Cb(r, c) = Cb(img.rows() - 1u, c);
        }
    }

    // padding (horizontally)
    for (size_t c = img.columns(); c < fullCols; ++c)
    {
        for (size_t r = 0; r < fullRows; ++r)
        {
            Yp(r, c) = Yp(r, img.columns() - 1u);
            Cr(r, c) = Cr(r, img.columns() - 1u);
            Cb(r, c) = Cb(r, img.columns() - 1u);
        }
    }
}

void PPMImg::display()
{
    for (size_t r = 0; r < fullRows; r++)
    {
        for (size_t c = 0; c < fullCols; ++c)
        {
            std::cout << "(" << int(Yp[r * fullCols + c]) << ", "
                << int(Cb[r * fullCols + c]) << ", "
                << int(Cr[r * fullCols + c]) << "), ";
        }
        std::cout << std::endl;
    }
}
