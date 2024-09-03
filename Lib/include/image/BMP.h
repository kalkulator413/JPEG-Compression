#include <cmath>
#include <math/Matrix.h>

class BMPImg {
public:
    size_t fullRows;
    size_t fullCols;

    BMPImg(const char * filename);
    ~BMPImg() = default;
    void display();

    Matrix<float> Yp;
    Matrix<float> Cb;
    Matrix<float> Cr;
};
