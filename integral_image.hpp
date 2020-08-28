#pragma once
#include "mat.hpp"
#include "mat_view_2d.hpp"

template <typename T>
class IntegralImage : public Mat<T> {
 public:
  template <typename E>
  IntegralImage(const MatView2D<E>& mat) : Mat<T>({mat.height(), mat.width()}) {
    for (unsigned y = 0; y < mat.height(); ++y) {
      T sum = 0;
      for (unsigned x = 0; x < mat.width(); ++x) {
        sum += mat[y][x];
        (*this)[y][x] = sum;
      }
    }

    for (unsigned x = 0; x < mat.width(); ++x) {
      T sum = 0;
      for (unsigned y = 0; y < mat.height(); ++y) {
        sum += (*this)[y][x];
        (*this)[y][x] = sum;
      }
    }
  }

  T getSum(unsigned x0, unsigned y0, unsigned x1, unsigned y1) {
    const T a = (x0 == 0 || y0 == 0) ? 0 : (*this)[y0 - 1][x0 - 1];
    const T b = (y0 == 0 || x1 == 0) ? 0 : (*this)[y0 - 1][x1];
    const T c = (y1 == 0 || x0 == 0) ? 0 : (*this)[y1][x0 - 1];
    const T d = (y1 == 0 || x1 == 0) ? 0 : (*this)[y1][x1];

    return d + a - b - c;
  }
};
