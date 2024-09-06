#include <iostream>
#include <image/PPM.h>

PPMImg::PPMImg(const char * filename)
    : Yp(0, 0), Cb(0, 0), Cr(0, 0)
{
    using namespace std;
    FILE* stream;
    stream = fopen(filename, "r");

    char c;
    int ch;

    // width, height, and pixel depth of image
    size_t cols, rows;
    int depth;
    cols = 0u;
    rows = 0u;
    depth = -1;

    // adapted from https://github.com/eshyong/PPM-Viewer/blob/master/ppm.cpp
    // and simplified
    
    // magic number P6
    c = char(getc(stream));
    if (c != 'P')
        cerr << "file is not in ppm format.\n";

    c = char(getc(stream));
    if (c != '6')
        cerr << "Wrong ppm format.\n";

    // num buffer for numbers in ASCII form
    char num[16];
    int val;
    int index = 0;

    // find width, height, and max color value
    do
    {
        c = char(getc(stream));
        switch (c) {
            case '#':
                // found a comment, ignore and skip to next line
                while ((char)getc(stream) != '\n');
                break;
            case '\n':
            case ' ':
                // no numbers found
                if (index == 0) break;

                // end of a number
                val = atoi(num);
                if (val != 0) {
                    // set any unset fields
                    if (cols == 0u) 
                        cols = val;
                    else if (rows == 0u) 
                        rows = val;
                    else 
                        depth = val;
                    // reset num
                    memset(num, 0, sizeof(num));
                    index = 0;
                }
                break;
            default:
                // add digit to number
                if (isdigit(c)) {
                    num[index] = c;
                    index++;
                }
                break;
        }
    }
    while ((cols == 0u || rows == 0u || depth == -1) && !feof(stream));

    int r, g, b;
    r = -1;
    g = -1;
    b = -1;
    // index for width and height
    size_t i = 0, j = 0;

    fullRows = std::ceil(rows / 8.0f) * 8u;
    fullCols = std::ceil(cols / 8.0f) * 8u;

    Yp = Matrix<float>(fullRows, fullCols);
    Cb = Matrix<float>(fullRows, fullCols);
    Cr = Matrix<float>(fullRows, fullCols);

    do
    {
        ch = getc(stream);
        
        if (r == -1) 
            r = ch;
        else if (g == -1)
            g = ch;
        else if (b == -1) {
            // all values are now found
            // alpha is always 255 since ppms only encode RGB
            b = ch;
            
            float y = 0.299f * r + 0.587f * g + 0.114f * b - 128;
            float cb = -0.1687f * r - 0.33126f * g + 0.5f * b;
            float cr = 0.5f * r - 0.41869f * g - 0.0813f * b;

            Yp(j, i) = y;
            Cb(j, i) = cb;
            Cr(j, i) = cr;

            if (i < (cols - 1)) {
                i++;
            } else {
                j++;
                i = 0;
            }
            r = -1;
            g = -1;
            b = -1;
        }
    }
    while (j < rows);

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
