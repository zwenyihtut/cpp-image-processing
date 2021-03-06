#pragma once
#include "mat.hpp"

Mat<uint8_t> canny(Mat<uint8_t>& input, uint8_t min, uint8_t max);
Mat<uint8_t> directionMap(const Mat<uint8_t>& input);
