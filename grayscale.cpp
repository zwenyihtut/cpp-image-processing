#include "grayscale.hpp"
#include "img.hpp"
#include "convolute.hpp"

Mat<uint8_t> grayscale(const Mat<uint8_t>& image) {
  const auto height = img::height(image);
  const auto width = img::width(image);
  const auto channel = std::min<unsigned>(img::channel(image), 3);
  Mat<uint8_t> output = { { height, width, 1 } };

  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      unsigned sum = 0;
      for (unsigned c = 0; c < channel; ++c) {
        sum += image[y][x][c];
      }
      output[y][x][0] = std::round(sum / 3.0);
    }
  }
  return output;
}
