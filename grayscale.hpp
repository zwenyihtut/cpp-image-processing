#pragma once
#include "typedefs.hpp"
#include "convolute.hpp"

namespace {
template <typename RGBImage>
Image<PixelG8> fromRGBToGrayscale(RGBImage& img);

template <typename RGBAImage>
Image<PixelG8> fromRGBAToGrayscale(const RGBAImage& img);

}  // namespace

/**
 * Turn an image into grayscale.
 *
 * @tparam GenericImage Input image type
 * @param img Input image
 * @returns Image<PixelG8> A grayscale image
 */
template <typename GenericImage>
Image<PixelG8> grayscale(const GenericImage& img);

/**
 * Clone an image.
 *
 * @tparam Image Input/Output image type
 * @param img Input image
 * @returns Output image with the same values
 */
template <typename Image>
Image copy(const Image& img);

/**
 * Blur an image.
 */
template <typename Image>
Image blur(const Image& img);

/**
 * Blur an image, but with gaussian weights.
 */
template <typename Image>
Image gaussian(const Image& img);

namespace {
template <typename RGBImage>
Image<PixelG8> fromRGBToGrayscale(RGBImage& img) {
  Image<PixelG8> ret(img.width(), img.height());

  for (size_t y = 0; y < img.height(); ++y) {
    for (size_t x = 0; x < img.width(); ++x) {
      const auto [r, g, b] = img[y][x].val();
      uint8_t avg = std::round((r + g + b) / 3.0);
      ret[y][x].val({ avg });
    }
  }

  return ret;
}

template <typename RGBAImage>
Image<PixelG8> fromRGBAToGrayscale(const RGBAImage& img) {
  Image<PixelG8> ret(img.width(), img.height());

  for (size_t y = 0; y < img.height(); ++y) {
    for (size_t x = 0; x < img.width(); ++x) {
      ////IGNORE("-Wunused-variable")
      const auto [r, g, b, _] = img[y][x].val();
      // END_IGNORE()

      uint8_t avg = std::round((r + g + b) / 3.0);
      ret[y][x].val({ avg });
    }
  }
  return ret;
}
}  // namespace

template <typename GenericImage>
Image<PixelG8> grayscale(const GenericImage& img) {
  if constexpr (GenericImage::pixel_t::CHANNELS == 3) {
    return fromRGBToGrayscale(img);
  } else if constexpr (GenericImage::pixel_t::CHANNELS == 4) {
    return fromRGBAToGrayscale(img);
  } else {
    return img.clone();
  }
}

template <typename Image>
Image copy(const Image& img) {
  Image cpy(img.width(), img.height());
  for (size_t y = 0; y < img.height(); ++y) {
    for (size_t x = 0; x < img.width(); ++x) {
      cpy[y][x].val(img[y][x].val());
    }
  }
  return cpy;
}

template <typename Image>
Image blur(const Image& img) {
  constexpr unsigned KERNAL_SIDE_LENGTH = 9;
  constexpr unsigned KERNAL_TOTAL_ELEMS =
      KERNAL_SIDE_LENGTH * KERNAL_SIDE_LENGTH;

  constexpr double KERNAL_WEIGHT = 1.0 / KERNAL_TOTAL_ELEMS;

  Mat<double> blurKernal(
      { KERNAL_SIDE_LENGTH, KERNAL_SIDE_LENGTH },
      std::vector<double>(KERNAL_TOTAL_ELEMS, KERNAL_WEIGHT));

  return convolute(img, blurKernal);
}

template <typename Image>
Image gaussian(const Image& img) {
  Matrix<1, 5> gaussianRowVector = { { { 0.0625, 0.25, 0.375, 0.25,
                                         0.0625 } } };

  Matrix<5, 1> gaussianColVector = {
    { { 0.0625 }, { 0.25 }, { 0.375 }, { 0.25 }, { 0.0625 } }
  };

  Image ret = img.clone();

  for (int i = 0; i < 1; ++i) {
    Image buffer(ret.width(), ret.height());
    convoluteToBuffer(ret, gaussianColVector, buffer);
    convoluteToBuffer(buffer.clone(), gaussianRowVector, buffer);
    ret = buffer.clone();
  }
  return ret;
}
