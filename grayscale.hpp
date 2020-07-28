#pragma once
#include "convolute.hpp"
#include "mat.hpp"

Mat<uint8_t> grayscale(const Mat<uint8_t>& image);
Mat<uint8_t> gaussian(const Mat<uint8_t>& image);

