#pragma once
#include <utility>
#include "mat.hpp"

std::pair<Mat<double>, Mat<double>> sobelXYGradients(const Mat<uint8_t>& input);
Mat<uint8_t> sobel(const Mat<uint8_t>& input);
