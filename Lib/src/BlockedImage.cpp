#include <cmath>
#include <cstring>
#include <image/BlockedImage.h>

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
            
            // TODO: handle padding. Right now i just copy over noise

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
