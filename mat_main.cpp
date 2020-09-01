#include <iomanip>
#include <iostream>

#include "box_filters.hpp"
#include "gaussian.hpp"
#include "img.hpp"
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

template <typename T, typename O>
void normalize2(Mat<T>& mat, O range_beg, O range_end) {
  O min_val = *std::min_element(mat.begin(), mat.end());
  O max_val = *std::max_element(mat.begin(), mat.end());

  std::transform(mat.begin(), mat.end(), mat.begin(),
                 [min_val, max_val, range_beg, range_end](T n) -> double {
                   double d = (range_end - range_beg) / (max_val - min_val) *
                                  (n - max_val) +
                              range_end;
                   return d;
                 });
}

template <typename V>
Mat<double> convolute(Mat<V>& input, const BoxFilter& boxFilter) {
  const auto WIDTH = img::width(input);
  const auto HEIGHT = img::height(input);
  const auto CHANNELS = img::channel(input);

  Mat<double> output = {{HEIGHT, WIDTH, CHANNELS}};
  MatView2D<uint8_t> view(input);

  IntegralImage sumTable(view);
  for (unsigned y = 4; y < HEIGHT - 4; ++y) {
    for (unsigned x = 4; x < WIDTH - 4; ++x) {
      for (unsigned c = 0; c < CHANNELS; ++c) {
        double sum = 0;
        for (auto& box : boxFilter) {
          const auto [x0, y0, x1, y1] = box.coordinates();
          sum +=
              box.val() *
              sumTable.getSum(x0 - 4 + x, y0 + y - 4, x1 + x - 4, y1 + y - 4);
        }
        output[y][x][c] = sum;
      }
    }
  }
  normalize2<double, double>(output, 0, 255);
  return output;
}

template <typename V>
Mat<double> convolute2(Mat<V>& input, const BoxFilter& boxFilter) {
  const auto WIDTH = img::width(input);
  const auto HEIGHT = img::height(input);
  const auto CHANNELS = img::channel(input);

  Mat<double> output = {{HEIGHT, WIDTH, CHANNELS}};
  MatView2D<uint8_t> view(input);

  IntegralImage sumTable(view);
  for (unsigned y = 7; y < HEIGHT - 7; ++y) {
    for (unsigned x = 7; x < WIDTH - 7; ++x) {
      for (unsigned c = 0; c < CHANNELS; ++c) {
        double sum = 0;
        for (auto& box : boxFilter) {
          const auto [x0, y0, x1, y1] = box.coordinates();
          sum +=
              box.val() *
              sumTable.getSum(x0 - 7 + x, y0 + y - 7, x1 + x - 7, y1 + y - 7);
        }
        output[y][x][c] = sum;
      }
    }
  }
  normalize2<double, double>(output, 0, 255);
  return output;
}

template <typename V>
Mat<double> convolute3(Mat<V>& input, const BoxFilter& boxFilter) {
  const auto WIDTH = img::width(input);
  const auto HEIGHT = img::height(input);
  const auto CHANNELS = img::channel(input);

  Mat<double> output = {{HEIGHT, WIDTH, CHANNELS}};
  MatView2D<uint8_t> view(input);

  IntegralImage sumTable(view);
  for (unsigned y = 10; y < HEIGHT - 10; ++y) {
    for (unsigned x = 10; x < WIDTH - 10; ++x) {
      for (unsigned c = 0; c < CHANNELS; ++c) {
        double sum = 0;
        for (auto& box : boxFilter) {
          const auto [x0, y0, x1, y1] = box.coordinates();
          sum +=
              box.val() *
              sumTable.getSum(x0 - 10 + x, y0 + y - 10, x1 + x - 10, y1 + y - 10);
        }
        output[y][x][c] = sum;
      }
    }
  }
  normalize2<double, double>(output, 0, 255);
  return output;
}
int main(int argc, char** argv) {
  auto image = img::read(argv[1]);
  auto outBX = convolute(image, makeGaussianBoxFilterXX(9));
  auto outBY = convolute(image, makeGaussianBoxFilterYY(9));

  img::write(gaussianXX(image), "gauss_filter_xx.png", PNG_COLOR_TYPE_GRAY);

  img::write(outBX.clone<uint8_t>(), "box_filter_xx.png", PNG_COLOR_TYPE_GRAY);
  img::write(outBY.clone<uint8_t>(), "box_filter_yy.png", PNG_COLOR_TYPE_GRAY);
}
