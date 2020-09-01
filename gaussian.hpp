#pragma once
#include "mat.hpp"
#include <iostream>

Mat<uint8_t> gaussian(const Mat<uint8_t>& image);

Mat<uint8_t> gaussian2nd(const Mat<uint8_t>& image);

Mat<uint8_t> gaussianXX(const Mat<uint8_t>& image);
Mat<uint8_t> gaussianYY(const Mat<uint8_t>& image);

Mat<uint8_t> gaussianX(const Mat<uint8_t>& image);
Mat<uint8_t> gaussianY(const Mat<uint8_t>& image);


class BoxFilter;

BoxFilter makeGaussianBoxFilterXY(unsigned size);
BoxFilter makeGaussianBoxFilterXX(unsigned size);
BoxFilter makeGaussianBoxFilterYY(unsigned size);

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
