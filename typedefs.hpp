#pragma once
#include <vector>
using SingleValueBuffer = Buffer<double, 1>;
using SingleValuePixel = typename SingleValueBuffer::pixel_t;
using SingleValueImage = Image<SingleValuePixel, SingleValueBuffer>;

using CoordinateBuffer = Buffer<unsigned, 2>;

template <typename Element>
using DynamicBuffer2D = std::vector<std::vector<Element>>;
