#pragma once
#include "neighbours.hpp"
#include "sobel.hpp"
#include "typedefs.hpp"
#include "utility.hpp"

Image<PixelG8> thinEdges(SingleValueImage& intensities,
                         SingleValueImage& directions) {
  Image<PixelG8> ret(intensities.width(), intensities.height());

  Image<PixelRGB8> directionMap(intensities.width(), intensities.height());
  for (unsigned y = 1; y < intensities.height() - 1; ++y) {
    for (unsigned x = 1; x < intensities.width() - 1; ++x) {

      const auto [intensity] = intensities[y][x].val();
      const auto [dir] = directions[y][x].val();

      const auto pos = findDirection(dir);
      const auto neg = findOppositeDirection(pos);

      if (intensity > 50) {
        switch (pos) {
          case Direction::TOP_LEFT:
            directionMap[y][x].val({ 0x80, 0, 0 });
            break;
          case Direction::BOTTOM_RIGHT:
            directionMap[y][x].val({ 0xe6, 0x19, 0x4b });
            break;

          case Direction::TOP:
            directionMap[y][x].val({ 0x9a, 0x63, 0x24 });
            break;

          case Direction::BOTTOM:
            directionMap[y][x].val({ 0xff, 0xe1, 0x19 });
            break;

          case Direction::LEFT:
            directionMap[y][x].val({ 0x46, 0x99, 0x90 });
            break;

          case Direction::RIGHT:
            directionMap[y][x].val({ 0, 0, 0x75 });
            break;

          case Direction::TOP_RIGHT:
            directionMap[y][x].val({ 0x42, 0xd4, 0xf4 });
            break;

          case Direction::BOTTOM_LEFT:
            directionMap[y][x].val({ 0x91, 0x1e, 0xb4 });
            break;
        }
      } else {
        directionMap[y][x].val({ 0, 0, 0 });
      }

      const auto [posX, posY] = getOffset(pos);
      const auto [negX, negY] = getOffset(neg);

      const auto [posIntensity] = intensities[y + posY][x + posX].val();
      const auto [negIntensity] = intensities[y + negY][x + negX].val();

      if (intensity > posIntensity && intensity >= negIntensity) {
        ret[y][x].val({ std::min(std::round(intensity), 255.0) });
      } else {
        ret[y][x].val({ 0 });
      }
    }
  }
  directionMap.buffer()->save("direction.png");
  return ret;
}

using StrongPixelBuffer = Buffer<bool, 1>;

StrongPixelBuffer doubleThreshold(Image<PixelG8>& img,
                                  uint8_t min,
                                  uint8_t max) {
  StrongPixelBuffer buf(img.width(), img.height(), 0);
  for (unsigned y = 0; y < img.height(); ++y) {
    for (unsigned x = 0; x < img.width(); ++x) {
      auto [val] = img[y][x].val();
      if (val < min)
        img[y][x].val({ 0 });
      else if (val > max)
        buf.buffer()[y][x] = true;
    }
  }
  return buf;
}

void trackEdges(Image<PixelG8>& img, const StrongPixelBuffer& strongPixels) {
  for (unsigned y = 1; y < img.height() - 1; ++y) {
    for (unsigned x = 1; x < img.width() - 1; ++x) {
      if (strongPixels.buffer()[y][x])  // we don't care about strong pixels
        continue;
      const auto [value] = img[y][x].val();
      if (value == 0)  // we don't care about "off" pixels
        continue;
      bool shouldBeOff = true;
      for (const auto& [nx, ny] : neighbours(x, y)) {
        if (strongPixels.buffer()[ny][nx]) {
          // weak pixels with strong pixel neighbours gets a pass
          shouldBeOff = false;
          break;
        }
      }
      if (shouldBeOff)
        img[y][x].val({ 0 });  // otherwise, remove the weak pixel
    }
  }
}

void removeBoundaryArtifacts(Image<PixelG8>& img) {
  const auto WIDTH = img.width();
  const auto HEIGHT = img.height();
  for (unsigned x = 0; x < WIDTH; ++x) {
    img[0][x].val({ 0 });
    img[HEIGHT - 1][x].val({ 0 });
  }
  for (unsigned y = 0; y < HEIGHT; ++y) {
    img[y][0].val({ 0 });
    img[y][WIDTH - 1].val({ 0 });
  }
}

Image<PixelG8> canny(Image<PixelG8>& img) {
  auto [intensities, directions] = intensityMap(img);

  SingleValueImage intensitiesView(intensities);
  SingleValueImage directionsView(directions);

  auto ret = thinEdges(intensitiesView, directionsView);
  const auto strongPixels = doubleThreshold(ret, 50, 180);
  trackEdges(ret, strongPixels);

  removeBoundaryArtifacts(ret);
  return ret;
}
