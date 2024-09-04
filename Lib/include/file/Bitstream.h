#include <fstream>

struct Bits
{
    Bits() = default;
    Bits(uint16_t _data, uint8_t _nBits) : nBits(_nBits), data(_data) {};
    uint8_t nBits;
    uint16_t data;
};

class BitStream
{
public:
    BitStream(const char* fname);
    void writeBits(Bits bits);
    void flush();
    void meanWrite(char c);
    void blockHeader(uint8_t id, uint16_t length);
    ~BitStream();
private:
    std::ofstream outfile;
    uint8_t nBits = 0;
    uint32_t buffer;
};
