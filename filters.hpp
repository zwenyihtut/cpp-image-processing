#pragma once
#include "matrix.hpp"

/**
 * Convolutes a given image with a kernal, and puts into a given buffer.
 *
 * Allows you to control the type of output buffer. In case of out-of-bound
 * pixel access (which will happen with any kernal bigger than 1x1),
 * The mirrored pixels are used. (E.g. image[0][-2] becomes image[0][2])
 *
 *
 * @tparam Image Input image type
 * @tparam Buffer Output buffer type
 * @tparam ROWS Kernal height
 * @tparam COLS Kernal width
 *
 * @param img Input image
 * @param kernal Kernal to use for convolution
 * @param buffer output buffer
 *
 * @returns void
 */
template <typename Image,
          typename Buffer,
          long unsigned ROWS,
          long unsigned COLS>
void filterToBuffer(const Image& img,
                    const Matrix<ROWS, COLS>& kernal,
                    Buffer& buffer);

/**
 * Convolutes a given image with a kernal, returning an Image of the same type.
 *
 * In case of out-of-bound pixel access (which will happen with any kernal
 * bigger than 1x1), The mirrored pixels are used.
 * (E.g. image[0][-2] becomes image[0][2])
 *
 *
 * @tparam Image Input image type
 * @tparam ROWS Kernal height
 * @tparam COLS Kernal width
 *
 * @param img Input image
 * @param kernal Kernal to use for convolution
 *
 * @returns The convolved image
 */
template <typename Image, long unsigned ROWS, long unsigned COLS>
Image filter(const Image& img, const Matrix<ROWS, COLS>& kernal);

/**
 * Convolutes a given image with a kernal, and puts into a given buffer.
 *
 * Allows you to control the type of output buffer. Skips the pixels near
 * the edges, whose convolution require out-of-bound pixel access.
 *
 * @tparam Image Input image type
 * @tparam Buffer Output buffer type
 * @tparam ROWS Kernal height
 * @tparam COLS Kernal width
 *
 * @param img Input image
 * @param kernal Kernal to use for convolution
 * @param buffer output buffer
 *
 * @returns void
 */
template <typename Image,
          typename Buffer,
          long unsigned ROWS,
          long unsigned COLS>
void filterToBufferUnchecked(const Image& img,
                             const Matrix<ROWS, COLS>& kernal,
                             Buffer& buffer);

/**
 * Convolutes a given image with a kernal, returning an Image of the same type.
 *
 * Skips the pixels near the edges, whose convolution require out-of-bound
 * pixel access.
 *
 *
 * @tparam Image Input image type
 * @tparam ROWS Kernal height
 * @tparam COLS Kernal width
 *
 * @param img Input image
 * @param kernal Kernal to use for convolution
 *
 * @returns The convolved image
 */
template <typename Image, long unsigned ROWS, long unsigned COLS>
Image filterUnchecked(const Image& img, const Matrix<ROWS, COLS>& kernal);

//----------------------- implementations --------------------------------
template <typename Image,
          typename Buffer,
          long unsigned ROWS,
          long unsigned COLS>
void filterToBuffer(const Image& img,
                    const Matrix<ROWS, COLS>& kernal,
                    Buffer& buffer) {
  const auto W = img.width();
  const auto H = img.height();

  constexpr auto CHANNELS = Image::pixel_t::CHANNELS;
  constexpr auto HALF_ROWS = ROWS / 2;
  constexpr auto HALF_COLS = COLS / 2;

  // If index is over the bound, turn it over the mirror
  const auto adjusted = [](int index, int end) -> int {
    return index > (end - 1) ? 2 * (end - 1) - index : std::abs(index);
  };
  for (unsigned y = 0; y < img.height(); ++y) {
    for (unsigned x = 0; x < img.width(); ++x) {
      for (unsigned channel = 0; channel < CHANNELS; ++channel) {
        double sum = 0;

        for (int i = (int)y - (int)HALF_ROWS, kernalY = 0;
             i <= int(y + HALF_ROWS); ++i, ++kernalY) {
          for (int j = (int)x - (int)HALF_COLS, kernalX = 0;
               j <= int(x + HALF_COLS); ++j, ++kernalX) {
            const double channelVal =
                img[adjusted(i, H)][adjusted(j, W)].get(channel);
            const double kernalVal = kernal[kernalY][kernalX];
            sum += channelVal * kernalVal;
          }
        }
        buffer[y][x].set(channel, sum);
      }
    }
  }
}

template <typename Image, long unsigned ROWS, long unsigned COLS>
Image filter(const Image& img, const Matrix<ROWS, COLS>& kernal) {
  const auto W = img.width();
  const auto H = img.height();

  Image buffer(W, H);
  filterToBuffer(img, kernal, buffer);
  return buffer;
}

template <typename Image,
          typename Buffer,
          long unsigned ROWS,
          long unsigned COLS>
void filterToBufferUnchecked(const Image& img,
                             const Matrix<ROWS, COLS>& kernal,
                             Buffer& buffer) {
  const auto W = img.width();
  const auto H = img.height();

  constexpr auto CHANNELS = Image::pixel_t::CHANNELS;
  constexpr auto HALF_ROWS = ROWS / 2;
  constexpr auto HALF_COLS = COLS / 2;

  const auto outOfBound = [W, H, HALF_ROWS, HALF_COLS](unsigned x,
                                                       unsigned y) -> bool {
    return int(x) - int(HALF_COLS) < 0 || int(y) - int(HALF_ROWS) < 0 ||
           x + HALF_COLS >= W || y + HALF_ROWS >= H;
  };
  for (unsigned y = 0; y < img.height(); ++y) {
    for (unsigned x = 0; x < img.width(); ++x) {
      if (outOfBound(x, y))
        continue;
      for (unsigned channel = 0; channel < CHANNELS; ++channel) {
        double sum = 0;
        for (unsigned i = y - HALF_ROWS, kernalY = 0; i <= y + HALF_ROWS;
             ++i, ++kernalY) {
          for (unsigned j = x - HALF_COLS, kernalX = 0; j <= x + HALF_COLS;
               ++j, ++kernalX) {
            sum += double(img[i][j].get(channel)) * kernal[kernalY][kernalX];
          }
        }
        buffer[y][x].set(channel, sum);
      }
    }
  }
}

template <typename Image, long unsigned ROWS, long unsigned COLS>
Image filterUnchecked(const Image& img, const Matrix<ROWS, COLS>& kernal) {
  const auto W = img.width();
  const auto H = img.height();

  Image buffer(W, H);
  filterToBufferUnchecked(img, kernal, buffer);
  return buffer;
}
