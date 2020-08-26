#include <iostream>
#include "integral_image.hpp"
#include "mat_view_2d.hpp"
#include "mat_view.hpp"

int main() {
  Mat<uint8_t> mat{ { 3, 4, 1 }, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } };
  MatView2D<uint8_t> view(mat);

  std::cout << (int)view[0][0] << std::endl;
  std::cout << (int)view[0][1] << std::endl;
  std::cout << (int)view[1][0] << std::endl;
  std::cout << (int)view[1][1] << std::endl;
  
  IntegralImage<int> table(view);
  
  for (int y = 0; y < table.dimension(0); ++y) {
    for (int x = 0; x < table.dimension(1); ++x) {
      std::cout << table[y][x] << ' ';
    }
    std::cout << '\n';
  }
}
