#include <iomanip>
#include <iostream>

#include "box_filters.hpp"
#include "gaussian.hpp"
#include "integral_image.hpp"

constexpr auto SIZE = 21;

void makeMat(const BoxFilter& filter) {
  Mat<int> mat({SIZE, SIZE});
  for (const auto& box : filter) {
    const auto [x0, y0, x1, y1] = box.coordinates();
    for (int x = x0; x <= x1; ++x) {
      for (int y = y0; y <= y1; ++y) {
        mat[y][x] = box.val();
      }
    }
  }

  for (int y = 0; y < mat.dimension(0); ++y) {
    for (int x = 0; x < mat.dimension(1); ++x) {
      std::cout << std::setw(3) << mat[y][x] << ' ';
    }
    std::cout << '\n';
  }
}

int main() {
  makeMat(makeGaussianBoxFilterXX(SIZE));
  std::cout << "------------------------------------\n";
  makeMat(makeGaussianBoxFilterXY(SIZE));
}
