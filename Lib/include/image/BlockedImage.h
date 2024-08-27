#include <cstdint>
#include <image/BMP.h>

class BlockedImage
{
public:
    using Block = Matrix<float>;
    static constexpr uint8_t BLOCK_SIZE = 8u;

    BlockedImage(BMPImg bmp);
    ~BlockedImage();
    void applyDCT();
private:
    size_t blockedCols;
    size_t blockedRows;
    size_t fullCols;
    size_t fullRows;
    Matrix<Block> blockMatrix;
};
