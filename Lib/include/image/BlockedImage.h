#include <cstdint>
#include <image/BMP.h>
#include <vector>

class BlockedImage
{
public:
    static constexpr uint8_t BLOCK_SIZE = 8u;
    size_t blockedCols;
    size_t blockedRows;
    size_t fullCols;
    size_t fullRows;
    BlockedImage(BMPImg bmp);
    std::vector<Matrix<float>> Y;
    std::vector<Matrix<float>> Cr;
    std::vector<Matrix<float>> Cb;
    std::vector<Matrix<uint8_t, 0>> qY;
    std::vector<Matrix<uint8_t, 0>> qCr;
    std::vector<Matrix<uint8_t, 0>> qCb;
    void applyDCT();
    void quantize();
};
