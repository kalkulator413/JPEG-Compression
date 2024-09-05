#include <cmath>
#include <math/Matrix.h>

class PPMImg {
public:
    size_t fullRows;
    size_t fullCols;

    PPMImg(const char * filename);
    ~PPMImg() = default;
    void display();

    Matrix<float> Yp;
    Matrix<float> Cb;
    Matrix<float> Cr;
};
