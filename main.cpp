extern "C" {
#include <png.h>
}
#include <cassert>
#include <cmath>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include "hhs/logger.h"

#include "canny.hpp"
#include "gaussian.hpp"
#include "grayscale.hpp"
#include "harris.hpp"
#include "img.hpp"
#include "sobel.hpp"

void run(const Mat<uint8_t>& image) {
  std::cout << "Height: " << img::height(image) << '\n'
            << "Width: " << img::width(image) << std::endl;
  auto grayscaledImage = grayscale(image);
  auto gauss = gaussian(grayscaledImage);
  auto corners = harris(grayscaledImage);

  Mat<uint8_t> harrisOutput = image;
  for (auto [x, y] : corners) {
    harrisOutput[y][x][0] = 255;
    harrisOutput[y][x][1] = 0;
    harrisOutput[y][x][2] = 0;
    harrisOutput[y][x][3] = 255;
  }

  img::write(harrisOutput, "harris.png");
  img::write(directionMap(gauss), "directions.png", PNG_COLOR_TYPE_RGB_ALPHA);
  img::write(gauss, "gauss.png", PNG_COLOR_TYPE_GRAY);
  img::write(sobel(gauss), "sobel.png", PNG_COLOR_TYPE_GRAY);
  img::write(canny(grayscaledImage, 50, 180), "canny.png", PNG_COLOR_TYPE_GRAY);
  img::write(gaussianXX(grayscaledImage), "gxx.png", PNG_COLOR_TYPE_GRAY);
  img::write(grayscaledImage, "grayscale.png", PNG_COLOR_TYPE_GRAY);
  
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <input>\n";
    return 1;
  }

  auto image = img::read(argv[1]);
  run(image);
  return 0;
}
