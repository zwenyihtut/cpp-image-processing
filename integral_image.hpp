#pragma once
#include <vector>

#include "mat_view_2d.hpp"

class IntegralImage {
 public:
  template <typename E>
  IntegralImage(const MatView2D<E>& mat)
      : mData(mat.height(), std::vector<int>(mat.width(), 0)) {

    for (unsigned y = 0; y < mat.height(); ++y) {
      int sum = 0;
      for (unsigned x = 0; x < mat.width(); ++x) {
        sum += mat[y][x];
        this->mData[y][x] = sum;
      }
    }

    for (unsigned x = 0; x < mat.width(); ++x) {
      int sum = 0;
      for (unsigned y = 0; y < mat.height(); ++y) {
        sum += this->mData[y][x];
        this->mData[y][x] = sum;
      }
    }
  }

  int getSum(unsigned x0, unsigned y0, unsigned x1, unsigned y1) const {
    const int a = (x0 == 0 || y0 == 0) ? 0 : this->mData[y0 - 1][x0 - 1];
    const int b = (y0 == 0 || x1 == 0) ? 0 : this->mData[y0 - 1][x1];
    const int c = (y1 == 0 || x0 == 0) ? 0 : this->mData[y1][x0 - 1];
    const int d = (y1 == 0 || x1 == 0) ? 0 : this->mData[y1][x1];

    return d + a - b - c;
  }

 private:
  std::vector<std::vector<int>> mData;
};
