#include <cmath>
#include "sobel.hpp"
#include "img.hpp"
#include "utility.hpp"

std::vector<std::pair<unsigned, unsigned>> harris(Mat<uint8_t>& input) {
  const auto [xIntensities, yIntensities] = sobelXYGradients(input);

  const auto width = img::width(input);
  const auto height = img::height(input);

  Mat<double> sum({ 2, 2 }, { 0, 0, 0, 0 });

  std::vector<std::pair<unsigned, unsigned>> coordinates;

  for (unsigned y = 2; y < height - 2; ++y) {
    for (unsigned x = 2; x < width - 2; ++x) {
      sum = { 0, 0, 0, 0 };

      for (unsigned dy = y - 2; dy < y + 2; ++dy) {
        for (unsigned dx = x - 2; dx < x + 2; ++dx) {
          const auto ix = xIntensities[dy][dx];
          const auto iy = yIntensities[dy][dx];

          sum += { ix * ix, ix * iy, ix * iy, iy * iy };
        }
      }

      const auto a = sum[0][0] / 25;
      const auto b = sum[0][1] / 25;
      const auto c = sum[1][0] / 25;
      const auto d = sum[1][1] / 25;

      const double f = 1;
      const double s = -(a + d);
      const double t = (a * d - b * c);

      const double lambda1 = (-s + std::sqrt(s * s - 4 * f * t)) / (2 * f);
      const double lambda2 = (-s - std::sqrt(s * s - 4 * f * t)) / (2 * f);

      const double trace = (lambda1 + lambda2);
      const double det = lambda1 * lambda2;
      const double R = det - (0.04 * trace * trace);

      if (R > 10e7) {
        coordinates.push_back({ x, y });
      }
    }
  }
  return coordinates;
}
