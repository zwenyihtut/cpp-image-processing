#include "canny.hpp"
#include "img.hpp"
#include "sobel.hpp"
#include "utility.hpp"

Mat<uint8_t> canny(Mat<uint8_t>& input, uint8_t min, uint8_t max);
std::pair<Mat<double>, Mat<double>> findGradients(const Mat<uint8_t>& input) {
  const auto [bufferX, bufferY] = sobelXYGradients(input);

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
      if (value != 128)
        continue;
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
