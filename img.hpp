#pragma once
#include <cassert>
#include <cstdint>
#include "libpng_wrapper.hpp"
#include "mat.hpp"
#include "utility.hpp"

namespace img {
template <typename V>
typename Mat<V>::size_type height(const Mat<V>& image) {
  return image.dimension(0);
}

template <typename V>
typename Mat<V>::size_type width(const Mat<V>& image) {
  return image.dimension(1);
}

template <typename V>
typename Mat<V>::size_type channel(const Mat<V>& image) {
  return image.dimension(2);
}

inline Mat<uint8_t> read(const std::string& name) {
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

  unsigned index = 0;
  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      for (unsigned c = 0; c < channels; ++c) {
        const auto value = img.buffer[y][x * channels + c];
        ret(index, value);
        ++index;
      }
    }
  }

  libpng::clean(img);
  return ret;
}

inline void write(const Mat<uint8_t>& image,
           const std::string& name,
           uint8_t type = PNG_COLOR_TYPE_RGB) {
  auto pngPtr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!pngPtr) {
    throw std::runtime_error("Unable to write image");
  }
  png_infop infoPtr = png_create_info_struct(pngPtr);

  const auto height = image.dimension(0);
  const auto width = image.dimension(1);
  const auto channel = image.dimension(2);

  libpng::HeaderChunk header = { width, height, type };
  libpng::writeHeaderChunk(pngPtr, infoPtr, header);

  // Allocate buffer
  auto rows = (png_bytepp)png_malloc(pngPtr, height * (sizeof(png_bytep)));
  auto rowbytes = png_get_rowbytes(pngPtr, infoPtr);

  for (png_uint_32 i = 0; i < height; i++) {
    rows[i] = (png_bytep)png_malloc(pngPtr, rowbytes);
  }

  unsigned index = 0;
  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      for (unsigned c = 0; c < channel; ++c) {
        const auto value = image(index);
        rows[y][x * channel + c] = value;
        ++index;
      }
    }
  }

  png_set_rows(pngPtr, infoPtr, rows);
  FILE* file = fopen(name.c_str(), "wb");
  png_init_io(pngPtr, file);
  png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
  fclose(file);
}

}  // namespace img
