WIP

Roadmap:
- [x] Set up project structure and cmake
- [x] Set up Math library w/ aligned data
- [x] Set up testing frameworks
- [x] Read `.bmp` files into memory
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
- [x] Format as JPEG & save

Dependencies:
- `catch2` - for building tests and benchmarks

All data in Data/Raw is obtained courtesy of [FileSampleHub](https://filesampleshub.com/format/image/bmp) and [University of Southern Carolina](https://people.math.sc.edu/Burkardt/data/bmp/bmp.html)
