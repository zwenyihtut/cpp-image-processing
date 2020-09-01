#include "gaussian.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

#include "box_filters.hpp"
#include "convolute.hpp"

Mat<uint8_t> gaussianX(const Mat<uint8_t>& image) {
  Mat<double> rowKernal = {{1, 5}, {0.0625, 0.25, 0.375, 0.25, 0.0625}};
  return convolute(image, rowKernal);
}

Mat<uint8_t> gaussianY(const Mat<uint8_t>& image) {
  Mat<double> colKernal = {{5, 1}, {0.0625, 0.25, 0.375, 0.25, 0.0625}};
  return convolute(image, colKernal);
}

Mat<uint8_t> gaussianXX(const Mat<uint8_t>& image) {
  Mat<double> cpy = image.clone<double>();
  Mat<double> kernal({1, 7}, {0.09, 0.41, 0, -1.0, 0, 0.41, 0.09});
  cpy = convolute(cpy, kernal);
  normalize<double>(cpy, 0, 255);
  return cpy.clone<uint8_t>();
}

Mat<uint8_t> gaussianYY(const Mat<uint8_t>& image) {
  Mat<double> cpy = image.clone<double>();
  Mat<double> kernal({7, 1}, {0.09, 0.41, 0, -1.0, 0, 0.41, 0.09});
  cpy = convolute(cpy, kernal);
  normalize<double>(cpy, 0, 255);
  return cpy.clone<uint8_t>();
}

Mat<uint8_t> gaussian2nd(const Mat<uint8_t>& image) {
  constexpr unsigned KERNAL_SIZE = 7;
  Mat<double> cpy = image.clone<double>();

  Mat<double> kernal({KERNAL_SIZE, KERNAL_SIZE},
                     [KERNAL_SIZE](unsigned i) -> double {
                       const double x = i % KERNAL_SIZE - 3.0;
                       const double y = i / KERNAL_SIZE - 3.0;
                       constexpr double sig = 1.0;
                       double k = (-1 / (M_PI * std::pow(sig, 4)));

                       return k * (1 - ((x * x + y * y) / (2 * sig * sig))) *
                              std::exp(-((x * x + y * y) / (2 * sig * sig)));
                     });
  cpy = convolute(cpy, kernal);
  normalize<double>(cpy, 0, 255);
  return cpy.clone<uint8_t>();
}

Mat<uint8_t> gaussian(const Mat<uint8_t>& image) {
  Mat<uint8_t> output = image;

  for (int i = 0; i < 1; ++i) {
    output = gaussianX(image);
    output = gaussianY(image);
  }
  return output;
}

BoxFilter makeGaussianBoxFilterXY(unsigned size) {
  assert(size % 3 == 0);

  constexpr unsigned REGIONS = 3;

  const auto sizePerRegion = size / REGIONS;

  const unsigned padding = size - 9;
  const unsigned paddingPerRegion = padding / 3;

  const unsigned squareSize = 3 + (padding / 3);
  const unsigned leftOver = (size - (2 * squareSize) - 1) / 2;

  BoxFilter guassianXYFilter;
  guassianXYFilter.addBox(Box(leftOver, leftOver, leftOver + squareSize - 1,
                              leftOver + squareSize - 1, 1));

  guassianXYFilter.addBox(
      Box(leftOver + squareSize + 1, leftOver + squareSize + 1,
          leftOver + 2 * squareSize, leftOver + 2 * squareSize, 1));

  guassianXYFilter.addBox(Box(leftOver, leftOver + squareSize + 1,
                              leftOver + squareSize - 1,
                              leftOver + 2 * squareSize, -1));

  guassianXYFilter.addBox(Box(leftOver + squareSize + 1, leftOver,
                              leftOver + 2 * squareSize,
                              leftOver + squareSize - 1, -1));
  return guassianXYFilter;
}

BoxFilter makeGaussianBoxFilterXX(unsigned size) {
  assert(size % 3 == 0);
  constexpr unsigned REGIONS = 3;
  constexpr unsigned LEFT_REGION_ORIG_WIDTH = 2;
  constexpr unsigned RIGHT_REGION_ORIG_WIDTH = 2;
  constexpr unsigned MID_REGION_ORIG_WIDTH = 5;

  const auto sizePerRegion = size / REGIONS;

  const unsigned padding = size - 9;
  const unsigned paddingPerRegion = padding / 3;

  const unsigned leftRegionWidth = LEFT_REGION_ORIG_WIDTH + paddingPerRegion;
  const unsigned rightRegionWidth = RIGHT_REGION_ORIG_WIDTH + paddingPerRegion;
  const unsigned middleRegionWidth = MID_REGION_ORIG_WIDTH + paddingPerRegion;

  BoxFilter guassianXXFilter;
  guassianXXFilter.addBox(Box(0, 0, leftRegionWidth - 1, size - 1, 0));
  guassianXXFilter.addBox(Box(leftRegionWidth, 0,
                              leftRegionWidth + middleRegionWidth - 1,
                              sizePerRegion - 1, -1));
  guassianXXFilter.addBox(Box(leftRegionWidth, sizePerRegion,
                              leftRegionWidth + middleRegionWidth - 1,
                              2 * sizePerRegion - 1, -2));
  guassianXXFilter.addBox(Box(leftRegionWidth, 2 * sizePerRegion,
                              leftRegionWidth + middleRegionWidth - 1,
                              3 * sizePerRegion - 1, -1));
  guassianXXFilter.addBox(
      Box(leftRegionWidth + middleRegionWidth, 0, size - 1, size - 1, 0));
  return guassianXXFilter;
}

BoxFilter makeGaussianBoxFilterYY(unsigned size) {
  auto filter = makeGaussianBoxFilterXX(size);
  filter.transpose();
  return filter;
}