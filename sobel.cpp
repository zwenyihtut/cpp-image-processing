#include "sobel.hpp"
#include "convolute.hpp"
#include "img.hpp"

std::pair<Mat<double>, Mat<double>> sobelXYGradients(
    const Mat<uint8_t>& input) {
  Mat<double> sobelXKernalRow = { { 1, 3 }, { 1, 0, -1 } };
  Mat<double> sobelXKernalCol = { { 3, 1 }, { 1, 2, 1 } };
  Mat<double> sobelYKernalRow = { { 1, 3 }, { 1, 2, 1 } };
  Mat<double> sobelYKernalCol = { { 3, 1 }, { -1, 0, 1 } };

  const auto height = img::height(input);
  const auto width = img::width(input);

  auto bufferX = convolute<uint8_t, double>(input, sobelXKernalRow);
  bufferX = convolute<double>(bufferX, sobelXKernalCol);
  auto bufferY = convolute<uint8_t, double>(input, sobelYKernalRow);
  bufferY = convolute<double>(bufferY, sobelYKernalCol);

  return { bufferX, bufferY };
}

Mat<uint8_t> sobel(const Mat<uint8_t>& input) {
  const auto [bufferX, bufferY] = sobelXYGradients(input);

  const auto height = img::height(input);
  const auto width = img::width(input);

  Mat<uint8_t> output = { { height, width, 1 } };

  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      unsigned val = std::hypot(bufferX[y][x][0], bufferY[y][x][0]);
      output[y][x][0] = static_cast<uint8_t>(std::min(val, 255u));
    }
  }
  return output;
}
