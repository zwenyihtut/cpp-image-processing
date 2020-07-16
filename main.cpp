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

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <input>\n";
    return 1;
  }

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
