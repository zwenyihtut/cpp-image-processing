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

#include "flatten.hpp"
#include "image.hpp"
#include "image_traverser.hpp"
#include "matrix.hpp"
#include "grayscale.hpp"
#include "pixels.hpp"
#include "sobel.hpp"
#include "canny.hpp"
#include "harris.hpp"
#include "utility.hpp"

#include "img.hpp"

std::ostream& operator<<(std::ostream& os, const PixelRGB8& pixel) {
  os << "(" << (unsigned)pixel.red() << ", " << (unsigned)pixel.green() << ", "
     << (unsigned)pixel.blue() << ")";
  return os;
}

template <typename Pixel>
void run(const std::string& filename) {
  ImageFileBuffer buffer(filename);
  auto img = Image<Pixel>(buffer);
  std::cout << "Image type: " << buffer.colorType() << '\n'
            << "Image resolution: " << img.info() << std::endl;

  Image<PixelG8> gs = grayscale(img);
  gs.buffer()->save("gray.png");
  harris(gs).buffer()->save("harris.png");

  auto blurred = blur(gs);
  blurred.buffer()->save("blur.png");
  auto gauss = canny(blurred);
  auto sbl = sobel(blurred);
  sbl.buffer()->save("sobel.png");
  gauss.buffer()->save("canny.png");
}

void run2(const Mat<uint8_t>& image) {
  auto grayscaledImage = img::grayscale(image);
  img::write(grayscaledImage, "grayscale.png", PNG_COLOR_TYPE_GRAY);
  auto gauss = img::gaussian(grayscaledImage);
  img::write(gauss, "gauss.png", PNG_COLOR_TYPE_GRAY);
  img::write(img::sobel(gauss), "sobel.png", PNG_COLOR_TYPE_GRAY);
  
  img::write(img::canny(grayscaledImage, 100, 150), "canny.png", PNG_COLOR_TYPE_GRAY);
  auto corners = img::harris(grayscaledImage);
  Mat<uint8_t> harrisOutput = image;
  for (auto [x, y] : corners) {
    harrisOutput[y][x][0] = 255;
    harrisOutput[y][x][1] = 0;
    harrisOutput[y][x][2] = 0;
  }
  img::write(harrisOutput, "harris.png");
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <input>\n";
    return 1;
  }

  auto image = img::read(argv[1]); 
  run2(image);
  return 0;
  std::shared_ptr<ImageFileBuffer> file(new ImageFileBuffer(argv[1]));

  if (file->colorType() == "RGBA") {
    run<PixelRGBA8>(argv[1]);
  } else if (file->colorType() == "RGB") {
    run<PixelRGB8>(argv[1]);
  } else if (file->colorType() == "GRAY" || file->colorType() == "PALETTE") {
    run<PixelG8>(argv[1]);
  } else {
    std::cout << "Unknown colortype: " << file->colorType() << std::endl;
  }
}
