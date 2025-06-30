# PPM To JPEG Compressor

## Installation Instructions

### Build From Source

Dependencies:
- `catch2` - for building tests and benchmarks

Clone the reposity and build locally:

```
git clone https://github.com/kalkulator413/JPEG-Compression.git
./build.sh
```

## Usage

```
./compress example.ppm example.jpg [quality]
```

`quality` should be a number between 1 and 100. If not specified, it defaults to 100.

 ## Roadmap

 ### Program Implementation
- [x] Set up project structure and cmake
- [x] Set up Math library w/ aligned data
- [x] Set up testing frameworks
- [x] Read `.ppm` files into memory
- [x] Change to YCbCr format
  - [ ] Try using SIMD?
- [ ] Investigate downscaling
- [x] Create 8x8 Chunks
- [x] Apply DCT on each chunk
  - [ ] Try threading on each chunk
  - [ ] Try using SIMD?
  - [ ] Maybe try a faster algorithm, like Arai, Agui and Nakajima: "A fast DCT-SQ scheme for images"
- [x] Quantize coeffs
  - [ ] Try using SIMD?
- [x] RLE on quantized values (running zig-zag)
- [x] Huffman coding
- [x] Format w/ JFIF & save

### Backlog
- [ ] Much needed performance improvements
  - Profile slowest methods
- [ ] Create releases on Github
- [ ] Add performance comparison benchmarks here

---

All data in Data/Raw is obtained courtesy of [FileSampleHub](https://filesamples.com/formats/ppm) and [University of Southern Carolina](https://people.sc.fsu.edu/~jburkardt/data/ppma/ppma.html)
