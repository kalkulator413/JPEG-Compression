#include <cmath>
#include <math/Matrix.h>

// Partially taken from stackoverflow user https://stackoverflow.com/users/5133242/liam and modified
class BMPImg {

private:
    size_t m_height;
    size_t m_width;
public:
    BMPImg(const char * filename);
    ~BMPImg();

    // these can technically all be stored in uint8_t*'s
    // but we eventually apply DCT which requires floats
    // and this format makes it nicer to avoid casts
    Matrix<float>* Yp;
    Matrix<float>* Cb;
    Matrix<float>* Cr;

    size_t width() {return m_width;}
    size_t height() {return m_height;}
};

inline BMPImg::~BMPImg()
{
    delete Yp;
    delete Cb;
    delete Cr;
}
