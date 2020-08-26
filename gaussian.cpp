#include "gaussian.hpp"
#include <cmath>
#include <iomanip>
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

template <typename T>
void normalize(Mat<T>& mat, T range_beg, T range_end) {
  T min_val = *std::min_element(mat.begin(), mat.end());
  T max_val = *std::max_element(mat.begin(), mat.end());

  std::transform(mat.begin(), mat.end(), mat.begin(),
                 [min_val, max_val, range_beg, range_end](T n) {
                   return (range_end - range_beg) / (max_val - min_val) *
                              (n - max_val) +
                          range_beg;
                 });
}

Mat<uint8_t> gaussianXX(const Mat<uint8_t>& image) {
  Mat<double> cpy = image.clone<double>();
  Mat<double> kernal({ 1, 7 }, { 0.09, 0.41, 0, -1.0, 0, 0.41, 0.09 });
  cpy = convolute(cpy, kernal);
  normalize<double>(cpy, 0, 255);
  return cpy.clone<uint8_t>();
}

Mat<uint8_t> gaussianYY(const Mat<uint8_t>& image) {
  Mat<double> cpy = image.clone<double>();
  Mat<double> kernal({ 7, 1 }, { 0.09, 0.41, 0, -1.0, 0, 0.41, 0.09 });
  cpy = convolute(cpy, kernal);
  normalize<double>(cpy, 0, 255);
  return cpy.clone<uint8_t>();
}

Mat<uint8_t> gaussian2nd(const Mat<uint8_t>& image) {
  constexpr unsigned KERNAL_SIZE = 7;
  Mat<double> cpy = image.clone<double>();

  Mat<double> kernal({ KERNAL_SIZE, KERNAL_SIZE },
                     [KERNAL_SIZE](unsigned i) -> double {
                       const double x = i % KERNAL_SIZE - 3.0;
                       const double y = i / KERNAL_SIZE - 3.0;
                       constexpr double sig = 1.0;
                       constexpr double k = (-1 / (M_PI * std::pow(sig, 4)));

                       return k * (1 - ((x * x + y * y) / (2 * sig * sig))) *
                              std::exp(-((x * x + y * y) / (2 * sig * sig)));
                     });
  cpy = convolute(cpy, kernal);
  normalize<double>(cpy, 0, 255);
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
