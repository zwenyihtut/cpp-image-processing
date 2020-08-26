#pragma once
#include <limits>
#include "img.hpp"
#include "mat.hpp"


template <typename T>
T clamp(T val, T min, T max) {
  if (val < min) return min;
  if (val > max) return max;
  return val;
}


/**
 * Convolutes a given image with a kernal, returning an Image of the same type.
 *
 * In case of out-of-bound pixel access (which will happen with any kernal
 * bigger than 1x1), The mirrored pixels are used.
 * (E.g. image[0][-2] becomes image[0][2])
 *
 *
 * @param img Input image
 * @param kernal Kernal to use for convolution
 *
 * @returns The convolved image
 */
template <typename V, typename O = V>
Mat<O> convolute(const Mat<V>& input, const Mat<double>& kernal) {
  const auto WIDTH = img::width(input);
  const auto HEIGHT = img::height(input);
  const auto CHANNELS = img::channel(input);

  const auto ROWS = kernal.dimension(0);
  const auto COLS = kernal.dimension(1);
  const auto HALF_ROWS = ROWS / 2;
  const auto HALF_COLS = COLS / 2;
  Mat<O> output = { { HEIGHT, WIDTH, CHANNELS } };

  // If index is over the bound, take the mirrored coordinate
  const auto mirrorIfNeeded = [](int index, int end) -> int {
    return index > (end - 1) ? 2 * (end - 1) - index : std::abs(index);
  };

  for (unsigned y = 0; y < HEIGHT; ++y) {
    for (unsigned x = 0; x < WIDTH; ++x) {
      for (unsigned c = 0; c < CHANNELS; ++c) {
        double sum = 0;

        for (int i = (int)y - (int)HALF_ROWS, kernalY = 0;
             i <= int(y + HALF_ROWS); ++i, ++kernalY) {
          for (int j = (int)x - (int)HALF_COLS, kernalX = 0;
               j <= int(x + HALF_COLS); ++j, ++kernalX) {
            const double channelVal =
                input[mirrorIfNeeded(i, HEIGHT)][mirrorIfNeeded(j, WIDTH)][c];
            const double kernalVal = kernal[kernalY][kernalX];
            sum += channelVal * kernalVal;
          }
        }
        if constexpr (std::is_unsigned<O>::value) {
          sum = clamp<double>(sum, 0, std::numeric_limits<O>::max());
        }
        output[y][x][c] = sum;
      }
    }
  }
  return output;
}


