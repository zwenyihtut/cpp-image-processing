#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>
#include "libpng_wrapper.hpp"
#include "mat.hpp"
#include "utility.hpp"

namespace img {
template <typename V>
typename Mat<V>::size_type height(const Mat<V>& image) {
  return image.dimension(0);
}

template <typename V>
typename Mat<V>::size_type width(const Mat<V>& image) {
  return image.dimension(1);
}

template <typename V>
typename Mat<V>::size_type channel(const Mat<V>& image) {
  return image.dimension(2);
}
Mat<uint8_t> read(const std::string& name) {
  auto img = libpng::read(name);
  const unsigned width = png_get_image_width(img.png, img.info);
  const unsigned height = png_get_image_height(img.png, img.info);

  const auto colorType = png_get_color_type(img.png, img.info);
  unsigned channels = 1;
  switch (colorType) {
    case PNG_COLOR_TYPE_RGB:
      channels = 3;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      channels = 4;
      break;
  }
  Mat<uint8_t> ret({ height, width, channels },
                   std::vector<uint8_t>(width * height * channels));

  unsigned index = 0;
  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      for (unsigned c = 0; c < channels; ++c) {
        const auto value = img.buffer[y][x * channels + c];
        ret(index, value);
        ++index;
      }
    }
  }

  libpng::clean(img);
  return ret;
}

void write(const Mat<uint8_t>& image,
           const std::string& name,
           uint8_t type = PNG_COLOR_TYPE_RGB) {
  auto pngPtr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!pngPtr) {
    throw std::runtime_error("Unable to write image");
  }
  png_infop infoPtr = png_create_info_struct(pngPtr);

  const auto height = image.dimension(0);
  const auto width = image.dimension(1);
  const auto channel = image.dimension(2);

  libpng::HeaderChunk header = { width, height, type };
  libpng::writeHeaderChunk(pngPtr, infoPtr, header);

  // Allocate buffer
  auto rows = (png_bytepp)png_malloc(pngPtr, height * (sizeof(png_bytep)));
  auto rowbytes = png_get_rowbytes(pngPtr, infoPtr);

  for (png_uint_32 i = 0; i < height; i++) {
    rows[i] = (png_bytep)png_malloc(pngPtr, rowbytes);
  }

  unsigned index = 0;
  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      for (unsigned c = 0; c < channel; ++c) {
        const auto value = image(index);
        rows[y][x * channel + c] = value;
        ++index;
      }
    }
  }

  png_set_rows(pngPtr, infoPtr, rows);
  FILE* file = fopen(name.c_str(), "wb");
  png_init_io(pngPtr, file);
  png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
  fclose(file);
}

Mat<uint8_t> grayscale(const Mat<uint8_t>& image) {
  const auto height = img::height(image);
  const auto width = img::width(image);
  const auto channel = std::min<unsigned>(img::channel(image), 3);
  Mat<uint8_t> output = { { height, width, 1 } };

  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      unsigned sum = 0;
      for (unsigned c = 0; c < channel; ++c) {
        sum += image[y][x][c];
      }
      output[y][x][0] = std::round(sum / 3.0);
    }
  }
  return output;
}

template <typename V, typename O = V>
Mat<O> convolute(const Mat<V>& input, const Mat<double>& kernal) {
  const auto WIDTH = img::width(input);
  const auto HEIGHT = img::height(input);
  const auto CHANNELS = img::channel(input);

  const auto ROWS = kernal.dimension(0);
  const auto COLS = kernal.dimension(1);
  const auto HALF_ROWS = ROWS / 2;
  const auto HALF_COLS = COLS / 2;
  Mat<O> output = { { HEIGHT, WIDTH, CHANNELS } };

  // If index is over the bound, turn it over the mirror
  const auto adjusted = [](int index, int end) -> int {
    return index > (end - 1) ? 2 * (end - 1) - index : std::abs(index);
  };
  for (unsigned y = 0; y < HEIGHT; ++y) {
    for (unsigned x = 0; x < WIDTH; ++x) {
      for (unsigned c = 0; c < CHANNELS; ++c) {
        double sum = 0;

        for (int i = (int)y - (int)HALF_ROWS, kernalY = 0;
             i <= int(y + HALF_ROWS); ++i, ++kernalY) {
          for (int j = (int)x - (int)HALF_COLS, kernalX = 0;
               j <= int(x + HALF_COLS); ++j, ++kernalX) {
            const double channelVal =
                input[adjusted(i, HEIGHT)][adjusted(j, WIDTH)][c];
            const double kernalVal = kernal[kernalY][kernalX];
            sum += channelVal * kernalVal;
          }
        }
        output[y][x][c] = sum;
      }
    }
  }
  return output;
}

std::pair<Mat<double>, Mat<double>> sobelXYGradients(const Mat<uint8_t>& input) {
  Mat<double> sobelXKernal = { { 3, 3 }, { 1, 0, -1, 2, 0, -2, 1, 0, -1 } };
  Mat<double> sobelYKernal = { { 3, 3 }, { -1, -2, -1, 0, 0, 0, 1, 2, 1 } };

  const auto height = img::height(input);
  const auto width = img::width(input);

  auto bufferX = convolute<uint8_t, double>(input, sobelXKernal);
  auto bufferY = convolute<uint8_t, double>(input, sobelYKernal);

  return { bufferX, bufferY };
}

Mat<uint8_t> sobel(const Mat<uint8_t>& input) {
  const auto [ bufferX, bufferY ] = sobelXYGradients(input);

  const auto height = img::height(input);
  const auto width = img::width(input);
  
  Mat<uint8_t> output = { { height, width, 1 } };

  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      unsigned val = std::hypot(bufferX[y][x][0], bufferY[y][x][0]);
      output[y][x][0] = static_cast<uint8_t>(std::min(val, 255u));
    }
  }
  return output;
}

std::pair<Mat<double>, Mat<double>> findGradients(const Mat<uint8_t>& input) {
  const auto [ bufferX, bufferY ] = sobelXYGradients(input);

  const auto height = img::height(input);
  const auto width = img::width(input);
  
  Mat<double> intensities = { { height, width } };
  Mat<double> directions = { { height, width } };

  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      const auto xValue = bufferX[y][x];
      const auto yValue = bufferY[y][x];
      intensities[y][x] = std::hypot(xValue, yValue);
      directions[y][x] = std::atan2(yValue, xValue);
    }
  }
  return { intensities, directions };
}

Mat<uint8_t> thinEdges(Mat<double>& intensities, Mat<double>& directions) {
  const auto height = intensities.dimension(0);
  const auto width = intensities.dimension(1);

  Mat<uint8_t> output = { { height, width } };

  for (unsigned y = 1; y < height - 1; ++y) {
    for (unsigned x = 1; x < width - 1; ++x) {
      const auto intensity = intensities[y][x];
      const auto theta = directions[y][x];

      const auto pos = findDirection(theta);
      const auto neg = findOppositeDirection(pos);

      const auto [posX, posY] = getOffset(pos);
      const auto [negX, negY] = getOffset(neg);

      const auto posIntensity = intensities[y + posY][x + posX];
      const auto negIntensity = intensities[y + negY][x + negX];

      if (intensity > posIntensity && intensity >= negIntensity) {
        output[y][x] = std::min(std::round(intensity), 255.0);
      } else {
        output[y][x] = 0;
      }
    }
  }
  return output;
}

void findStrongAndWeakPixels(Mat<uint8_t>& input, uint8_t min, uint8_t max) {
  for (uint8_t& value : input) {
    if (value < min) {
      value = 0;
    } else if (value < max) {
      value = 128;
    } else {
      value = 255;
    }
  }
}

void removeIsolatedWeakPixels(Mat<uint8_t>& input) {
  const auto height = img::height(input);
  const auto width = img::width(input);

  for (unsigned y = 1; y < height - 1; ++y) {
    for (unsigned x = 1; x < width - 1; ++x) {
      const auto value = input[y][x][0];
      if (value != 128) continue;
      bool shouldBeOff = true;
      for (unsigned ny = y - 1; ny < y + 1; ++ny) {
        for (unsigned nx = x - 1; nx < x + 1; ++nx) {
          const auto neighbour = input[ny][nx][0];
          if (neighbour == 255) {
            shouldBeOff = false;
            break;
          }
        }
      }
      input[y][x][0] = shouldBeOff ? 0 : 255;
    }
  }
}

void removeBoundaryArtifacts(Mat<uint8_t>& input) {
  const auto height = img::height(input);
  const auto width = img::width(input);
  for (unsigned y = 0; y < height; ++y) {
    input[y][0][0] = 0;
    input[y][width - 1][0] = 0;
  }

  for (unsigned x = 0; x < width; ++x) {
    input[0][x][0] = 0;
    input[height - 1][x][0] = 0;
  }
}

Mat<uint8_t> canny(Mat<uint8_t>& input, uint8_t min, uint8_t max) {
  auto [intensities, directions] = findGradients(input);

  auto output = thinEdges(intensities, directions);
  findStrongAndWeakPixels(output, min, max);
  removeIsolatedWeakPixels(output);
  removeBoundaryArtifacts(output);

  return output;
}

std::vector<std::pair<unsigned, unsigned>> harris(Mat<uint8_t>& input) {
  const auto [xIntensities, yIntensities] = sobelXYGradients(input);

  const auto width = img::width(input);
  const auto height = img::height(input);

  Mat<double> sum({2, 2}, {0, 0, 0, 0});
  
  std::vector<std::pair<unsigned, unsigned>> coordinates;

  for (unsigned y = 2; y < height - 2; ++y) {
    for (unsigned x = 2; x < width - 2; ++x) {
      sum = {0, 0, 0, 0};

      for (unsigned dy = y - 2; dy < y + 2; ++dy) {
        for (unsigned dx = x - 2; dx < x + 2; ++dx) {
          const auto ix = xIntensities[dy][dx];
          const auto iy = yIntensities[dy][dx];

          sum += {ix * ix, ix * iy, ix * iy, iy * iy};
        }
      }

      const auto a = sum[0][0] / 25;
      const auto b = sum[0][1] / 25;
      const auto c = sum[1][0] / 25;
      const auto d = sum[1][1] / 25;

      const double f = 1;
      const double s = -(a + d);
      const double t = (a * d - b * c);

      const double lambda1 = (-s + std::sqrt(s * s - 4 * f * t)) / (2 * f);
      const double lambda2 = (-s - std::sqrt(s * s - 4 * f * t)) / (2 * f);

      const double trace = (lambda1 + lambda2);
      const double det = lambda1 * lambda2;
      const double R = det - (0.04 * trace * trace);

      if (R > 10e7) {
        coordinates.push_back({ x, y });
      }
    }
  }
  return coordinates;
}

Mat<uint8_t> gaussian(const Mat<uint8_t>& image) {
  Mat<double> rowKernal = { { 1, 5 }, { 0.0625, 0.25, 0.375, 0.25, 0.0625 } };
  Mat<double> colKernal = { { 5, 1 }, { 0.0625, 0.25, 0.375, 0.25, 0.0625 } };

  Mat<uint8_t> output = image;

  for (int i = 0; i < 1; ++i) {
    output = convolute(output, rowKernal);
    output = convolute(output, colKernal);
  }
  return output;
}

}  // namespace img
