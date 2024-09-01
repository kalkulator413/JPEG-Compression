#include <cmath>
#include <cstring>
#include <image/BlockedImage.h>

// 1 / sqrt(2)
constexpr float DCT_COEFF = 0.707106781f;
constexpr float PI = 3.1415926535f;
static Matrix<Matrix<float>> coeffs(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);

// avoid division by storing inverses!!
static float qTableY[8*8] =
{ 1.f/16, 1.f/11, 1.f/10, 1.f/16, 1.f/24, 1.f/40, 1.f/51, 1.f/61,
    1.f/12, 1.f/12, 1.f/14, 1.f/19, 1.f/26, 1.f/58, 1.f/60, 1.f/55,
    1.f/14, 1.f/13, 1.f/16, 1.f/24, 1.f/40, 1.f/57, 1.f/69, 1.f/56,
    1.f/14, 1.f/17, 1.f/22, 1.f/29, 1.f/51, 1.f/87, 1.f/80, 1.f/62,
    1.f/18, 1.f/22, 1.f/37, 1.f/56, 1.f/68,1.f/109,1.f/103, 1.f/77,
    1.f/24, 1.f/35, 1.f/55, 1.f/64, 1.f/81, 1.f/104, 1.f/113, 1.f/92,
    1.f/49, 1.f/64, 1.f/78, 1.f/87, 1.f/103, 1.f/121, 1.f/120, 1.f/101,
    1.f/72, 1.f/92, 1.f/95, 1.f/98, 1.f/112, 1.f/100, 1.f/103, 1.f/99
};
static float qTableC[8*8] =
{ 1.f/17, 1.f/18, 1.f/24, 1.f/47, 1.f/99, 1.f/99, 1.f/99, 1.f/99,
    1.f/18, 1.f/21, 1.f/26, 1.f/66, 1.f/99, 1.f/99, 1.f/99, 1.f/99,
    1.f/24, 1.f/26, 1.f/56, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99,
    1.f/47, 1.f/66, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99,
    1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99,
    1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99,
    1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99,
    1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99, 1.f/99
};


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

template<>
void BlockedImage::quantize<true>()
{
    qY.reserve(Y.size());
    qCb.reserve(Cb.size());
    qCr.reserve(Cr.size());

    // TODO: try SIMD
    size_t nChunks = blockedRows * blockedCols;
    size_t chunkSize = BlockedImage::BLOCK_SIZE * BlockedImage::BLOCK_SIZE;
    for (size_t i = 0; i < nChunks; ++i)
    {
        Matrix<int16_t> newY(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
        Matrix<int16_t> newCr(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
        Matrix<int16_t> newCb(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
        for (size_t j = 0; j < chunkSize; ++j)
        {
            newY[j] = round(Y[i][j] * qTableY[j]);
            newCb[j] = round(Cb[i][j] * qTableC[j]);
            newCr[j] = round(Cr[i][j] * qTableC[j]);
        }
        qY.push_back(newY);
        qCb.push_back(newCb);
        qCr.push_back(newCr);
    }
}

template<>
void BlockedImage::quantize<false>()
{
    qY.reserve(Y.size());
    qCb.reserve(Cb.size());
    qCr.reserve(Cr.size());

    // TODO: try SIMD
    size_t nChunks = blockedRows * blockedCols;
    size_t chunkSize = BlockedImage::BLOCK_SIZE * BlockedImage::BLOCK_SIZE;
    for (size_t i = 0; i < nChunks; ++i)
    {
        Matrix<int16_t> newY(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
        Matrix<int16_t> newCr(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
        Matrix<int16_t> newCb(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);
        for (size_t j = 0; j < chunkSize; ++j)
        {
            newY[j] = round(Y[i][j] * qTableY[j]);
            newCb[j] = round(Cb[i][j] * qTableC[j]);
            newCr[j] = round(Cr[i][j] * qTableC[j]);
        }
        qY.push_back(newY);
        qCb.push_back(newCb);
        qCr.push_back(newCr);
    }

    Y.clear();
    Cb.clear();
    Cr.clear();
}
