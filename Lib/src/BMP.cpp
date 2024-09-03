#include <fstream>
#include <iostream>
#include <array>
#include <cassert>
#include <image/BMP.h>

// Partially taken from stackoverflow user https://stackoverflow.com/users/5133242/liam and modified
BMPImg::BMPImg(const char * filename)
    : Yp(0, 0), Cb(0, 0), Cr(0, 0)
{
    using namespace std;

    ifstream inf(filename);
    if(!inf)
        cerr<< "Unable to open file: " << filename << "\n";

    uint8_t m_bmpFileHeader[14];
    uint8_t m_bmpInfoHeader[40];

    unsigned char a;
    for(size_t i = 0; i < 14; i++)
    {
        inf >> hex >> a;
        m_bmpFileHeader[i] = a;
    }
    if(m_bmpFileHeader[0] != 'B' || m_bmpFileHeader[1] != 'M')
        cerr<<"Your info header might be different!\nIt should start with 'BM'.\n";

    unsigned int * array_offset_ptr = (unsigned int *)(m_bmpFileHeader + 10);
    size_t m_pixelArrayOffset = *array_offset_ptr;

    if( m_bmpFileHeader[11] != 0 || m_bmpFileHeader[12] !=0 || m_bmpFileHeader[13] !=0 )
        std::cerr<< "You probably need to fix something. bmp.h("<<__LINE__<<")\n";

    for(size_t i = 0; i < 40; i++)
    {
        inf >> hex >> a;
        m_bmpInfoHeader[i] = a;
    }

    int * width_ptr = (int*)(m_bmpInfoHeader+4);
    int * height_ptr = (int*)(m_bmpInfoHeader+8);

    size_t cols = *width_ptr;
    size_t rows = std::abs(*height_ptr);

    size_t m_bitsPerPixel = m_bmpInfoHeader[14];
    if(m_bitsPerPixel != 24)
        cerr<<"This program is for 24bpp files. Your bmp is not that\n";

    int compressionMethod = m_bmpInfoHeader[16];
    if(compressionMethod != 0)
    {
        cerr<<"There's some compression stuff going on that we might not be able to deal with.\n";
        cerr<<"Comment out offending lines to continue anyways. bpm.h line: "<<__LINE__<<"\n";
    }

    size_t m_rowSize = size_t( std::floor( (m_bitsPerPixel*cols + 31.)/32 ) ) * 4;
    size_t m_pixelArraySize = m_rowSize* rows;

    uint8_t* m_pixelData = new uint8_t[m_pixelArraySize];

    inf.seekg(m_pixelArrayOffset,ios::beg);
    for(size_t i=0;i<m_pixelArraySize;i++) {
        inf >> hex >> a;
        m_pixelData[i] = a;
    }

    // output is in rgb order.
    auto getPixel = [&] (size_t x, size_t y) {
        assert(x < cols && y < rows);

        std::array<uint8_t, 3> v;

        y = rows -1- y; //to flip things
        //std::cout<<"y: "<<y<<" x: "<<x<<"\n";
        v[0] = uint8_t( m_pixelData[ m_rowSize*y+3*x+2 ] ); //red
        v[1] = uint8_t( m_pixelData[ m_rowSize*y+3*x+1 ] ); //green
        v[2] = uint8_t( m_pixelData[ m_rowSize*y+3*x+0 ] ); //blue

        return v;
    };

    fullRows = std::ceil(rows / 8.0f) * 8u;
    fullCols = std::ceil(cols / 8.0f) * 8u;

    Yp = Matrix<float>(fullRows, fullCols);
    Cb = Matrix<float>(fullRows, fullCols);
    Cr = Matrix<float>(fullRows, fullCols);

    // todo: use SIMD for this
    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < cols; ++c)
        {
            auto [rd, gr, bl] = getPixel(c, r);

            Yp(r, c) = 0.299f * rd + 0.587f * gr + 0.114f * bl - 128;
            Cb(r, c) = -0.168736f * rd - 0.331264f * gr + 0.5f * bl;
            Cr(r, c) = 0.5f * rd - 0.418688f * gr - 0.081312f * bl;
        }
    }

    // padding (vertically)
    for (size_t r = rows; r < fullRows; ++r)
    {
        for (size_t c = 0; c < cols; ++c)
        {
            Yp(r, c) = Yp(rows - 1u, c);
            Cr(r, c) = Cr(rows - 1u, c);
            Cb(r, c) = Cb(rows - 1u, c);
        }
    }

    // padding (horizontally)
    for (size_t c = cols; c < fullCols; ++c)
    {
        for (size_t r = 0; r < fullRows; ++r)
        {
            Yp(r, c) = Yp(r, cols - 1u);
            Cr(r, c) = Cr(r, cols - 1u);
            Cb(r, c) = Cb(r, cols - 1u);
        }
    }

    delete[] m_pixelData;
}

void BMPImg::display()
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
