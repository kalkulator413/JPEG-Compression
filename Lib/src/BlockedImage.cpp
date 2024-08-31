#include <cmath>
#include <cstring>
#include <image/BlockedImage.h>

// 1 / sqrt(2)
constexpr float DCT_COEFF = 0.707106781f;
constexpr float PI = 3.1415926535f;
static Matrix<Matrix<float>> coeffs(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);

BlockedImage::BlockedImage(BMPImg bmp)
    : fullCols(bmp.fullCols), fullRows((bmp.fullRows))
{
    blockedCols = fullCols / 8u;
    blockedRows = fullRows / 8u;

    Y.reserve(blockedRows * blockedCols);
    Cr.reserve(blockedRows * blockedCols);
    Cb.reserve(blockedRows * blockedCols);

    for (size_t r = 0; r < fullRows; r += BlockedImage::BLOCK_SIZE)
    {
        for (size_t c = 0; c < fullCols; c += BlockedImage::BLOCK_SIZE)
        {
            Matrix<float> currY(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
            Matrix<float> currCr(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
            Matrix<float> currCb(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);

            for (size_t j = 0; j < BlockedImage::BLOCK_SIZE; ++j)
            {
                std::memcpy(& (currY(j, 0)), & (bmp.Yp(r + j, c)), BlockedImage::BLOCK_SIZE * sizeof(float));
                std::memcpy(& (currCr(j, 0)), & (bmp.Cr(r + j, c)), BlockedImage::BLOCK_SIZE * sizeof(float));
                std::memcpy(& (currCb(j, 0)), & (bmp.Cb(r + j, c)), BlockedImage::BLOCK_SIZE * sizeof(float));
            }

            Y.push_back(currY);
            Cr.push_back(currCr);
            Cb.push_back(currCb);
        }
    }
}

void dct8x8(Matrix<float>& pixels, Matrix<Matrix<float>>& coeffs)
{
    // TODO: try SIMD?
    Matrix<float> freqs = Matrix<float>(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
    for (size_t u = 0; u < BlockedImage::BLOCK_SIZE; ++u)
    {
        for (size_t v = 0; v < BlockedImage::BLOCK_SIZE; ++v)
        {
            float sum = 0.f;
            for (size_t x = 0; x < BlockedImage::BLOCK_SIZE; ++x)
            {
                for (size_t y = 0; y < BlockedImage::BLOCK_SIZE; ++y)
                {
                    sum += pixels(x, y) * coeffs(u, v)(x, y);
                }
            }
            freqs(u, v) = sum;
        }
    }
    pixels = freqs;
}

void BlockedImage::applyDCT()
{
    // TODO: try SIMD?
    for (size_t u = 0; u < BlockedImage::BLOCK_SIZE; ++u)
    {
        for (size_t v = 0; v < BlockedImage::BLOCK_SIZE; ++v)
        {
            Matrix<float> curr = Matrix<float>(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);;
            float coeff;
            for (size_t x = 0; x < BlockedImage::BLOCK_SIZE; ++x)
            {
                for (size_t y = 0; y < BlockedImage::BLOCK_SIZE; ++y)
                {
                    coeff = 0.25f * std::cos( (2*x + 1) * u * PI * 0.0625f) * std::cos( (2*y + 1) * v * PI * 0.0625);
                    if (u == 0)
                        coeff *= DCT_COEFF;
                    if (v == 0)
                        coeff *= DCT_COEFF;
                    curr(x, y) = coeff;
                }
            }
            coeffs(u, v) = curr;
        }
    }

    size_t numChunks = blockedCols * blockedRows;
    for (size_t i = 0; i < numChunks; ++i)
    {
        dct8x8(Y[i], coeffs);
        dct8x8(Cr[i], coeffs);
        dct8x8(Cb[i], coeffs);
    }
}
