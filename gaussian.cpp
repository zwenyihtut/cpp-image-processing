#include "gaussian.hpp"
#include <cmath>
#include <iostream>
#include "convolute.hpp"

Mat<uint8_t> gaussianX(const Mat<uint8_t>& image) {
  Mat<double> rowKernal = { { 1, 5 }, { 0.0625, 0.25, 0.375, 0.25, 0.0625 } };
  return convolute(image, rowKernal);
}

Mat<uint8_t> gaussianY(const Mat<uint8_t>& image) {
  Mat<double> colKernal = { { 5, 1 }, { 0.0625, 0.25, 0.375, 0.25, 0.0625 } };
  return convolute(image, colKernal);
}

Mat<uint8_t> gaussianXX(const Mat<uint8_t>& image) {
  Mat<double> cpy = image.clone<double>();
  const auto generator = [](double x) -> double {
    x -= 3;
    constexpr double sig = 1;

    return ((x * x / std::pow(sig, 4)) - (1 / std::pow(sig, 2))) *
           std::exp(-x * x / (2 * std::pow(sig, 2)));
  };
  // Mat<double> kernal({ 1, 7 }, generator);
   Mat<double> kernal({ 1, 7 }, { 0.09, 0.41, 0, -1.0, 0, 0.41, 0.09 });
  //Mat<double> kernal(
  //    { 9, 9 },
  //    { 0, 1, 1, 2, 2, 2,   1,   1,   0, 1, 2, 4, 5, 5,   5,   4,   2,
  //      1, 1, 4, 5, 3, 0,   3,   5,   4, 1, 2, 5, 3, -12, -24, -12, 3,
  //      5, 2, 2, 5, 0, -24, -40, -24, 0, 5, 2, 2, 5, 3,   -12, -24, -12,
  //      3, 5, 2, 1, 4, 5,   3,   0,   3, 5, 4, 1, 1, 2,   4,   5,   5,
  //      5, 4, 2, 1, 0, 1,   1,   2,   2, 2, 1, 1, 0

  //    });
  cpy = convolute(cpy, kernal);
  double min = 1000000.0;
  double max = -100000.0;
  for (double n : cpy) {
    if (min > n) min = n;
    if (max < n) max = n;
  }
  for (double& n : cpy) {
    n = ((n - min) * 255.0) / (max - min);
  }
  return cpy.clone<uint8_t>();
}

Mat<uint8_t> gaussian(const Mat<uint8_t>& image) {
  Mat<uint8_t> output = image;

  for (int i = 0; i < 1; ++i) {
    output = gaussianX(image);
    output = gaussianY(image);
  }
  return output;
}
