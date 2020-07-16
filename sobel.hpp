#pragma once
#include "convolute.hpp"
#include "pixels.hpp"

std::array<Image<DoublePixel, DoubleBuffer>, 2> sobelGetXandYIntensities(
    const Image<PixelG8>& img) {
  Mat<double> sobelXKernal = { { 3, 3 }, { 1, 0, -1, 2, 0, -2, 1, 0, -1 } };
  Mat<double> sobelYKernal = { { 3, 3 }, { -1, -2, -1, 0, 0, 0, 1, 2, 1 } };

  Image<DoublePixel, DoubleBuffer> xBuffer(img.width(), img.height());
  Image<DoublePixel, DoubleBuffer> yBuffer(img.width(), img.height());

  convoluteToBuffer(img, sobelXKernal, xBuffer);
  convoluteToBuffer(img, sobelYKernal, yBuffer);

  return { std::move(xBuffer), std::move(yBuffer) };
}

std::pair<SingleValueBuffer, SingleValueBuffer> intensityMap(
    const Image<PixelG8>& img) {
  Mat<double> sobelXKernal = { { 3, 3 }, { 1, 0, -1, 2, 0, -2, 1, 0, -1 } };
  Mat<double> sobelYKernal = { { 3, 3 }, { -1, -2, -1, 0, 0, 0, 1, 2, 1 } };

  Image<DoublePixel, DoubleBuffer> xBuffer(img.width(), img.height());
  Image<DoublePixel, DoubleBuffer> yBuffer(img.width(), img.height());

  convoluteToBuffer(img, sobelXKernal, xBuffer);
  convoluteToBuffer(img, sobelYKernal, yBuffer);

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
  Mat<double> sobelXKernal = { { 3, 3 }, { 1, 0, -1, 2, 0, -2, 1, 0, -1 } };
  Mat<double> sobelYKernal = { { 3, 3 }, { -1, -2, -1, 0, 0, 0, 1, 2, 1 } };

  Image<DoublePixel, DoubleBuffer> xBuffer(img.width(), img.height());
  Image<DoublePixel, DoubleBuffer> yBuffer(img.width(), img.height());

  convoluteToBuffer(img, sobelXKernal, xBuffer);
  convoluteToBuffer(img, sobelYKernal, yBuffer);

  Image<PixelG8> ret(img.width(), img.height());

  for (unsigned y = 0; y < img.height(); ++y) {
    for (unsigned x = 0; x < img.width(); ++x) {
      unsigned val = std::hypot(xBuffer[y][x].val(), yBuffer[y][x].val());
      ret[y][x].val({ static_cast<uint8_t>(std::min(val, 255u)) });
    }
  }
  return ret;
}
