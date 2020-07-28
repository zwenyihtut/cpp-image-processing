#pragma once
#include "mat.hpp"
#include <vector>

std::vector<std::pair<unsigned, unsigned>> harris(Mat<uint8_t>& input);
