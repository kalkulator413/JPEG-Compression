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
    std::vector<Matrix<int16_t>> qY;
    std::vector<Matrix<int16_t>> qCr;
    std::vector<Matrix<int16_t>> qCb;
    void applyDCT();

    template<bool benchmark=false>
    void quantize(uint8_t quality);
    
    void encode(uint8_t quality, const char* outfile);
};
