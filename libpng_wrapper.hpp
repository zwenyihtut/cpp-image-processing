#pragma once
extern "C" {
#include <png.h>
}
#include <cstdint>
#include <string>
#include <stdexcept>
#include <utility>

namespace libpng {

struct Image {
  png_structp png;
  png_infop info;
  png_bytepp buffer;
};

struct HeaderChunk {
  size_t width, height;

  uint8_t colorType = PNG_COLOR_TYPE_RGB;

  uint8_t bitDepth = 8;
  uint8_t interlaceType = PNG_INTERLACE_NONE;
  uint8_t compressionType = PNG_COMPRESSION_TYPE_DEFAULT;
  uint8_t filterMethod = PNG_FILTER_TYPE_DEFAULT;
};

void writeHeaderChunk(png_structp pngPtr,
                      png_infop infoPtr,
                      const HeaderChunk& header);
std::pair<png_structp, png_infop> initializeWriteStructAndInfoPtr();
bool verifyFormat(FILE* file);

Image read(const std::string& name);
void clean(Image& img);
}  // namespace libpng
