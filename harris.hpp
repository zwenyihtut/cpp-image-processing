#pragma once
#include "canny.hpp"
#include "mat.hpp"
#include "neighbours.hpp"
#include "pixels.hpp"
#include "sobel.hpp"
#include "utility.hpp"
#include <cmath>

Image<PixelG8> harris(Image<PixelG8> &img) {
  const auto [xIntensities, yIntensities] = sobelGetXandYIntensities(img);

  const auto WIDTH = img.width();
  const auto HEIGHT = img.height();
  Image<PixelRGB8> anotherOne = mapImage<Image<PixelG8>, Image<PixelRGB8>>(
      img,
      [](const PixelG8 &pix) -> typename Image<PixelRGB8>::pixel_t::value_t {
        const auto [v] = pix.val();
        return {v, v, v};
      });

  Mat<double> sum({2, 2}, {0, 0, 0, 0});

  Image<PixelG8> newImg(img.width(), img.height());
  for (unsigned y = 2; y < HEIGHT - 2; ++y) {
    for (unsigned x = 2; x < WIDTH - 2; ++x) {
      sum = {0, 0, 0, 0};

      for (unsigned dy = y - 2; dy < y + 2; ++dy) {
        for (unsigned dx = x - 2; dx < x + 2; ++dx) {
          const auto ix = xIntensities[dy][dx].val();
          const auto iy = yIntensities[dy][dx].val();

          sum += {ix * ix, ix * iy, ix * iy, iy * iy};
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
        newImg[y][x].val({255});
        anotherOne[y][x].val({255, 0, 0});
      }
    }
  }
  anotherOne.buffer()->save("corners.png");
  return newImg;
}
