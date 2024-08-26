#include <cassert>
#include <cmath>
#include <cstdint>

// Partially taken from stackoverflow user https://stackoverflow.com/users/5133242/liam and modified
class BMPImg {

private:
    size_t m_height;
    size_t m_width;
public:
    BMPImg(const char * filename);
    ~BMPImg();

    uint8_t* redData;
    uint8_t* greenData;
    uint8_t* blueData;

    size_t width() {return m_width;}
    size_t height() {return m_height;}

    void display();
};

inline BMPImg::~BMPImg() {
    delete[] redData;
    delete[] greenData;
    delete[] blueData;
}