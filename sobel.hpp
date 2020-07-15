#pragma once
#include "pixels.hpp"

std::array<Image<DoublePixel, DoubleBuffer>, 2> sobelGetXandYIntensities(
    const Image<PixelG8>& img) {
  Matrix<3, 3> sobelXKernal = { { { 1, 0, -1 }, { 2, 0, -2 }, { 1, 0, -1 } } };
  Matrix<3, 3> sobelYKernal = { { { -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 } } };

  Image<DoublePixel, DoubleBuffer> xBuffer(img.width(), img.height());
  Image<DoublePixel, DoubleBuffer> yBuffer(img.width(), img.height());

  filterToBuffer(img, sobelXKernal, xBuffer);
  filterToBuffer(img, sobelYKernal, yBuffer);

  return { std::move(xBuffer), std::move(yBuffer) };
}

std::pair<SingleValueBuffer, SingleValueBuffer> intensityMap(
    const Image<PixelG8>& img) {
  Matrix<3, 3> sobelXKernal = { { { 1, 0, -1 }, { 2, 0, -2 }, { 1, 0, -1 } } };
  // Y kernal is inverted along x-axis because of the way the pixels are indiced
  // along the y-axis
  Matrix<3, 3> sobelYKernal = { { { -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 } } };

  Image<DoublePixel, DoubleBuffer> xBuffer(img.width(), img.height());
  Image<DoublePixel, DoubleBuffer> yBuffer(img.width(), img.height());

  filterToBuffer(img, sobelXKernal, xBuffer);
  filterToBuffer(img, sobelYKernal, yBuffer);

  SingleValueBuffer intensities(img.width(), img.height(), 0);
  SingleValueBuffer directions(img.width(), img.height(), 0);

  SingleValueImage intensitiesView(intensities);
  SingleValueImage directionsView(directions);

  for (unsigned y = 0; y < img.height(); ++y) {
    for (unsigned x = 0; x < img.width(); ++x) {
      const auto xVal = xBuffer[y][x].val();
      const auto yVal = yBuffer[y][x].val();
      intensitiesView[y][x].val({ std::hypot(xVal, yVal) });
      directionsView[y][x].val({ std::atan2(yVal, xVal) });
    }
  }
  return { intensities, directions };
}

Image<PixelG8> sobel(const Image<PixelG8>& img) {
  Matrix<3, 3> sobelXKernal = { { { 1, 0, -1 }, { 2, 0, -2 }, { 1, 0, -1 } } };
  Matrix<3, 3> sobelYKernal = { { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } } };

  Image<DoublePixel, DoubleBuffer> xBuffer(img.width(), img.height());
  Image<DoublePixel, DoubleBuffer> yBuffer(img.width(), img.height());

  filterToBuffer(img, sobelXKernal, xBuffer);
  filterToBuffer(img, sobelYKernal, yBuffer);

  Image<PixelG8> ret(img.width(), img.height());

  for (unsigned y = 0; y < img.height(); ++y) {
    for (unsigned x = 0; x < img.width(); ++x) {
      unsigned val = std::hypot(xBuffer[y][x].val(), yBuffer[y][x].val());
      ret[y][x].val({ static_cast<uint8_t>(std::min(val, 255u)) });
    }
  }
  return ret;
}

