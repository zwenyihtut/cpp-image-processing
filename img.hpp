#pragma once
#include <cstdint>
#include "libpng_wrapper.hpp"
#include "mat.hpp"

Mat<uint8_t> read(const std::string& name) {
  auto img = libpng::read(name);
  const unsigned width = png_get_image_width(img.png, img.info);
  const unsigned height = png_get_image_height(img.png, img.info);

  const auto colorType = png_get_color_type(img.png, img.info);
  unsigned channels = 1;
  switch (colorType) {
    case PNG_COLOR_TYPE_RGB:
      channels = 3;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      channels = 4;
      break;
  }
  Mat<uint8_t> ret({ height, width, channels },
                   std::vector<uint8_t>(width * height * channels));
  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      for (unsigned c = 0; c < channels; ++c) {
        ret[y][x][c] = img.buffer[y][x * channels + c];
      }
    }
  }

  libpng::clean(img);
  return ret;
}
