WIP

Roadmap:
- [x] Set up project structure and cmake
- [x] Set up Math library w/ aligned data
- [x] Set up testing frameworks
- [x] Read `.bmp` files into memory
- [x] Change to YCbCr format
  - [ ] Benchmark against SIMD implementation
- [ ] Create 8x8 Chunks
- [ ] Apply DCT on each chunk
  - [ ] Try threading on each chunk
- [ ] Quantize coeffs
  - [ ] Benchmark against SIMD implementation
- [ ] RLE on quantized values (running zig-zag)
- [ ] Huffman coding
- [ ] Format as JPEG
- [ ] Investigate if decompression is necessary

Dependencies:
- `catch2` - for building tests

All data in Data/Raw is obtained courtesy of [FileSampleHub](https://filesampleshub.com/format/image/bmp) and [University of Southern Carolina](https://people.math.sc.edu/Burkardt/data/bmp/bmp.html)
