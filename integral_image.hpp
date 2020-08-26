#pragma once
#include "mat.hpp"
#include "mat_view_2d.hpp"

template <typename T>
class IntegralImage : public Mat<T> {
 public:
  template <typename E>
  IntegralImage(const MatView2D<E>& mat) : Mat<T>({ mat.height(), mat.width() }) {

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
};
