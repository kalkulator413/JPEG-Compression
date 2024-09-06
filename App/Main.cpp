#include "image/BlockedImage.h"
#include <iostream>
 
int main(int argc, char** argv) {

    if (argc != 3 && argc != 4)
    {
        std::cerr << "the arguments look weird" << std::endl;
        return -1;
    }

    uint8_t QUALITY = 100;
    if (argc == 4)
        QUALITY = atoi(argv[3]);

    PPMImg ppm(argv[1]);
    BlockedImage b(ppm);
    b.applyDCT();
    b.quantize(QUALITY);
    std::cout << "created " << argv[2] << std::endl;
    b.encode(QUALITY, argv[2]);

    return 0;
}
