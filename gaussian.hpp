#pragma once
#include "mat.hpp"

Mat<uint8_t> gaussian(const Mat<uint8_t>& image);

Mat<uint8_t> gaussian2nd(const Mat<uint8_t>& image);

Mat<uint8_t> gaussianXX(const Mat<uint8_t>& image);
Mat<uint8_t> gaussianYY(const Mat<uint8_t>& image);

Mat<uint8_t> gaussianX(const Mat<uint8_t>& image);
Mat<uint8_t> gaussianY(const Mat<uint8_t>& image);


class BoxFilter;

BoxFilter makeGaussianBoxFilterXY(unsigned size);
BoxFilter makeGaussianBoxFilterXX(unsigned size);
BoxFilter makeGaussianBoxFilterYY(unsigned size);