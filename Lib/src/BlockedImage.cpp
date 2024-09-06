#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <image/BlockedImage.h>
#include <file/Bitstream.h>

// 1 / sqrt(2)
constexpr float DCT_COEFF = 0.707106781f;
constexpr float PI = 3.1415926535f;
static Matrix<Matrix<float>> coeffs(BlockedImage::BLOCK_SIZE, BlockedImage::BLOCK_SIZE);

static uint8_t qTableY[8*8] =
{
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68,109,103, 77,
    24, 35, 55, 64, 81,104,113, 92,
    49, 64, 78, 87,103,121,120,101,
    72, 92, 95, 98,112,100,103, 99
};

static uint8_t qTableC[8*8] =
{
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

// static Huffman code tables from JPEG standard Annex K
// ref from https://github.com/stbrumme/toojpeg/blob/master/toojpeg.cpp#L62C4-L62C57
const uint8_t DcLuminanceCodesPerBitsize[16]   = { 0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0 };   // sum = 12
const uint8_t DcLuminanceValues         [12]   = { 0,1,2,3,4,5,6,7,8,9,10,11 };         // => 12 codes
const uint8_t AcLuminanceCodesPerBitsize[16]   = { 0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,125 }; // sum = 162
const uint8_t AcLuminanceValues        [162]   =                                        // => 162 codes
{
    0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,0x14,0x32,0x81,0x91,0xA1,0x08, // 16*10+2 symbols because
    0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,0x24,0x33,0x62,0x72,0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28, // upper 4 bits can be 0..F
    0x29,0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59, // while lower 4 bits can be 1..A
    0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89, // plus two special codes 0x00 and 0xF0
    0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6, // order of these symbols was determined empirically by JPEG committee
    0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,
    0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA
};
// Huffman definitions for second DC/AC tables (chrominance / Cb and Cr channels)
const uint8_t DcChrominanceCodesPerBitsize[16] = { 0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0 };   // sum = 12
const uint8_t DcChrominanceValues         [12] = { 0,1,2,3,4,5,6,7,8,9,10,11 };         // => 12 codes (identical to DcLuminanceValues)
const uint8_t AcChrominanceCodesPerBitsize[16] = { 0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,119 }; // sum = 162
const uint8_t AcChrominanceValues        [162] =                                        // => 162 codes
{
    0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91, // same number of symbol, just different order
    0xA1,0xB1,0xC1,0x09,0x23,0x33,0x52,0xF0,0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,0x19,0x1A,0x26, // (which is more efficient for AC coding)
    0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,
    0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,
    0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,
    0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA
};
const int16_t CodeWordLimit = 2048; // +/-2^11, maximum value after DCT
const uint8_t ZigZagInv[8*8] =
{
    0, 1, 8,16, 9, 2, 3,10,
    17,24,32,25,18,11, 4, 5,
    12,19,26,33,40,48,41,34,
    27,20,13, 6, 7,14,21,28,
    35,42,49,56,57,50,43,36,
    29,22,15,23,30,37,44,51,
    58,59,52,45,38,31,39,46,
    53,60,61,54,47,55,62,63
};

BlockedImage::BlockedImage(PPMImg ppm)
    : fullCols(ppm.fullCols), fullRows((ppm.fullRows))
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
                std::memcpy(& (currY(j, 0)), & (ppm.Yp(r + j, c)), BlockedImage::BLOCK_SIZE * sizeof(float));
                std::memcpy(& (currCr(j, 0)), & (ppm.Cr(r + j, c)), BlockedImage::BLOCK_SIZE * sizeof(float));
                std::memcpy(& (currCb(j, 0)), & (ppm.Cb(r + j, c)), BlockedImage::BLOCK_SIZE * sizeof(float));
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
void BlockedImage::quantize<true>(uint8_t quality)
{
    // formula from libjpeg
    quality = quality < 50 ? 5000 / quality : 200 - quality * 2;
    // store inverses to avoid division
    float qInvY[64];
    float qInvC[64];
    for (size_t i = 0u; i < 64u; ++i)
    {
        uint16_t lum = std::clamp<uint16_t>((qTableY[i] * quality + 50) / 100, 1u, 255u);
        uint16_t chrom = std::clamp<uint16_t>((qTableC[i] * quality + 50) / 100, 1u, 255u);
        qInvY[i] = 1.f / lum;
        qInvC[i] = 1.f / chrom;
    }

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
            newY[j] = round(Y[i][j] * qInvY[j]);
            newCb[j] = round(Cb[i][j] * qInvC[j]);
            newCr[j] = round(Cr[i][j] * qInvC[j]);
        }
        qY.push_back(newY);
        qCb.push_back(newCb);
        qCr.push_back(newCr);
    }
}

template<>
void BlockedImage::quantize<false>(uint8_t quality)
{
    // formula from libjpeg
    quality = quality < 50 ? 5000 / quality : 200 - quality * 2;
    // store inverses to avoid division
    float qInvY[64];
    float qInvC[64];
    for (size_t i = 0u; i < 64u; ++i)
    {
        uint16_t lum = std::clamp<uint16_t>((qTableY[i] * quality + 50) / 100, 1u, 255u);
        uint16_t chrom = std::clamp<uint16_t>((qTableC[i] * quality + 50) / 100, 1u, 255u);
        qInvY[i] = 1.f / lum;
        qInvC[i] = 1.f / chrom;
    }

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
            newY[j] = round(Y[i][j] * qInvY[j]);
            newCb[j] = round(Cb[i][j] * qInvC[j]);
            newCr[j] = round(Cr[i][j] * qInvC[j]);
        }
        qY.push_back(newY);
        qCb.push_back(newCb);
        qCr.push_back(newCr);
    }

    Y.clear();
    Cb.clear();
    Cr.clear();
}

void genTable(const uint8_t numCodes[16], const uint8_t* values, Bits result[256])
{
  uint16_t huffmanCode = 0u;
  for (uint8_t numBits = 1u; numBits <= 16u; numBits++)
  {
    for (uint8_t i = 0u; i < numCodes[numBits - 1]; i++)
      result[*values++] = Bits(huffmanCode++, numBits);
    huffmanCode <<= 1;
  }
}

void encode8x8(Matrix<int16_t>& chunk, int16_t& prevDC, BitStream& b,
    const Bits huffmanDC[256], const Bits huffmanAC[256], const Bits* codewords)
{
    int diff = chunk[0] - prevDC;
    if (diff == 0)
        b.writeBits(huffmanDC[0x00]);
    else
    {
        Bits bits = codewords[diff];
        b.writeBits(huffmanDC[bits.nBits]);
        b.writeBits(bits);
    }
    prevDC = chunk[0];

    // process AC stuff
    uint8_t lastNonZero = 0u;
    for (uint8_t i = 0u; i < 64u; i++)
    {
        if (chunk[ZigZagInv[i]] != 0)
            lastNonZero = i;
    }

    uint16_t offset = 0u;
    for (uint8_t i = 1u; i <= lastNonZero; i++)
    {
        while (chunk[ZigZagInv[i]] == 0)
        {
            offset += 0x10u;
            if (offset > 0xF0u)
            {
                b.writeBits(huffmanAC[0xF0]);
                offset = 0u;
            }
            i++;
        }

        Bits encoded = codewords[chunk[ZigZagInv[i]]];
        b.writeBits(huffmanAC[offset + encoded.nBits]);
        b.writeBits(encoded);
        offset = 0u;
    }

    if (lastNonZero < 63u)
        b.writeBits(huffmanAC[0x00]);
}

void BlockedImage::encode(uint8_t quality, const char* outfile)
{
    BitStream b(outfile);

    // SOI marker
    b.meanWrite(0xFF);
    b.meanWrite(0xD8);

    // JFIF APP0 tag
    b.meanWrite(0xFF);
    b.meanWrite(0XE0);

    // length of header
    b.meanWrite(0);
    b.meanWrite(16);

    // JFIF identifier & version
    b.meanWrite('J');
    b.meanWrite('F');
    b.meanWrite('I');
    b.meanWrite('F');
    b.meanWrite(0);
    b.meanWrite(1);
    b.meanWrite(1);

    // density stuff idk
    b.meanWrite(0);
    b.meanWrite(0);
    b.meanWrite(1);
    b.meanWrite(0);
    b.meanWrite(1);

    // no thumbnail
    b.meanWrite(0);
    b.meanWrite(0);

    // write quantization tables
    b.blockHeader(0xDB, 2 + 2 * (1 + 8*8));

    // formula from libjpeg
    quality = quality < 50 ? 5000 / quality : 200 - quality * 2;
    uint16_t scaledY[64];
    uint16_t scaledC[64];
    for (size_t i = 0u; i < 64u; ++i)
    {
        scaledY[i] = std::clamp<uint16_t>((qTableY[i] * quality + 50) / 100, 1u, 255u);
        scaledC[i] = std::clamp<uint16_t>((qTableC[i] * quality + 50) / 100, 1u, 255u);
    }

    b.meanWrite(0x00);
    for (size_t i = 0u; i < 64u; ++i)
        b.meanWrite(scaledY[ZigZagInv[i]]);

    b.meanWrite(0x01);
    for (size_t i = 0u; i < 64u; ++i)
        b.meanWrite(scaledC[ZigZagInv[i]]);

    // write image info
    b.blockHeader(0xC0, 2+6+3*3);
    // # of bits per channel
    b.meanWrite(0x08);
    // image dimensions
    b.meanWrite(fullRows >> 8);
    b.meanWrite(fullRows & 0xFF);
    b.meanWrite(fullCols >> 8);
    b.meanWrite(fullCols & 0xFF);

    // sampling & quantization tables
    b.meanWrite(3);
    for (uint8_t idx = 1; idx <= 3; ++idx)
    {
        b.meanWrite(idx);
        b.meanWrite(0x11);
        b.meanWrite(idx == 1 ? 0 : 1);
    }

    // huffman tables
    b.blockHeader(0xC4, 2+208+208);

    b.meanWrite(0x00);
    for (uint8_t c : DcLuminanceCodesPerBitsize)
        b.meanWrite(c);
    for (uint8_t c : DcLuminanceValues)
        b.meanWrite(c);

    b.meanWrite(0x10);
    for (uint8_t c : AcLuminanceCodesPerBitsize)
        b.meanWrite(c);
    for (uint8_t c : AcLuminanceValues)
        b.meanWrite(c);

    b.meanWrite(0x01);
    for (uint8_t c : DcChrominanceCodesPerBitsize)
        b.meanWrite(c);
    for (uint8_t c : DcChrominanceValues)
        b.meanWrite(c);

    b.meanWrite(0x11);
    for (uint8_t c : AcChrominanceCodesPerBitsize)
        b.meanWrite(c);
    for (uint8_t c : AcChrominanceValues)
        b.meanWrite(c);

    b.blockHeader(0xDA, 2+1+2*3+3);
    b.meanWrite(3);
    for (uint8_t idx = 1u; idx <= 3u; idx++)
    {
        b.meanWrite(idx);
        b.meanWrite(idx == 1 ? 0x00 : 0x11);
    }
    b.meanWrite(0x00);
    b.meanWrite(0x63);
    b.meanWrite(0x00);

    // run length encoding + huffman

    // generate huffman tables
    Bits huffmanLuminanceDC[256];
    Bits huffmanLuminanceAC[256];
    Bits huffmanChrominanceDC[256];
    Bits huffmanChrominanceAC[256];
    genTable(DcLuminanceCodesPerBitsize, DcLuminanceValues, huffmanLuminanceDC);
    genTable(AcLuminanceCodesPerBitsize, AcLuminanceValues, huffmanLuminanceAC);
    genTable(DcChrominanceCodesPerBitsize, DcChrominanceValues, huffmanChrominanceDC);
    genTable(AcChrominanceCodesPerBitsize, AcChrominanceValues, huffmanChrominanceAC);

    // precompute codewords
    Bits codewordsArray[2 * CodeWordLimit];
    Bits* codewords = &codewordsArray[CodeWordLimit]; // allow negative indices
    uint8_t numBits = 1;
    int32_t mask = 1;
    for (int16_t value = 1; value < CodeWordLimit; value++)
    {
        if (value > mask)
        {
            numBits++;
            mask = (mask << 1) | 1; // append a bit
        }
        codewords[-value] = Bits(mask - value, numBits);
        codewords[+value] = Bits(value, numBits);
    }

    // encode all Y blocks, then Cb, then Cr
    size_t nChunks = blockedRows * blockedCols;
    int16_t prevYDC = 0;
    int16_t prevCbDC = 0;
    int16_t prevCrDC = 0;
    for (size_t i = 0u; i < nChunks; ++i)
    {
        encode8x8(qY[i], prevYDC, b, huffmanLuminanceDC, huffmanLuminanceAC, codewords);
        encode8x8(qCb[i], prevCbDC, b, huffmanChrominanceDC, huffmanChrominanceAC, codewords);
        encode8x8(qCr[i], prevCrDC, b, huffmanChrominanceDC, huffmanChrominanceAC, codewords);
    }

    b.flush();
    
    // EOI marker
    b.meanWrite(0xFF);
    b.meanWrite(0xD9);
}
