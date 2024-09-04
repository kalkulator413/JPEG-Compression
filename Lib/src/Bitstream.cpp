#include <file/Bitstream.h>

BitStream::BitStream(const char* fname)
    : outfile(fname), nBits(0), buffer(0) {}

void BitStream::writeBits(Bits bits)
{
    nBits += bits.nBits;
    buffer = buffer << bits.nBits;
    buffer = buffer | bits.data;

    while (nBits >= 8)
    {
        nBits -= 8;
        uint8_t byte = uint8_t (buffer >> nBits);

        outfile.put(byte);

        // coincidental block marker
        if (byte == 0xFF)
            outfile.put(0x00);
    }
}

void BitStream::flush()
{
    // fill gaps with 1s
    writeBits(Bits(0x7f, 7u));
}

void BitStream::blockHeader(uint8_t id, uint16_t length)
{
    outfile.put(0xFF);
    outfile.put(id);
    outfile.put(uint8_t(length >> 8)); // first byte
    outfile.put(uint8_t(length & 0xFF)); // second byte
}

void BitStream::meanWrite(char c)
{
    outfile.put(c);
}

BitStream::~BitStream()
{
    outfile.close();
}
