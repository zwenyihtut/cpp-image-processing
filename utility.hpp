#pragma once
#include <cmath>
#include "assertions.hpp"

enum class Direction {
  TOP_LEFT,
  TOP,
  TOP_RIGHT,
  RIGHT,
  BOTTOM_RIGHT,
  BOTTOM,
  BOTTOM_LEFT,
  LEFT,
};

inline Direction findDirection(double radian) {
  if (std::abs(radian) < M_PI / 8) {
    return Direction::RIGHT;
  }
  if (radian >= M_PI / 8 && radian < 3 * M_PI / 8) {
    return Direction::TOP_RIGHT;
  }
  if (radian >= 3 * M_PI / 8 && radian < 5 * M_PI / 8) {
    return Direction::TOP;
  }
  if (radian >= 5 * M_PI / 8 && radian < 7 * M_PI / 8) {
    return Direction::TOP_LEFT;
  }
  if (std::abs(radian) > 7 * M_PI / 8) {
    return Direction::LEFT;
  }
  if (radian <= -M_PI / 8 && radian > -3 * M_PI / 8) {
    return Direction::BOTTOM_RIGHT;
  }
  if (radian <= -3 * M_PI / 8 && radian > -5 * M_PI / 8) {
    return Direction::BOTTOM;
  }
  if (radian <= -5 * M_PI / 8 && radian > -7 * M_PI / 8) {
    return Direction::BOTTOM_LEFT;
  }

  ASSERT_NOT_REACHED();
  return Direction::TOP;
}

inline Direction findOppositeDirection(Direction direction) {
  switch (direction) {
    case Direction::TOP_LEFT:
      return Direction::BOTTOM_RIGHT;

    case Direction::BOTTOM_RIGHT:
      return Direction::TOP_LEFT;

    case Direction::TOP:
      return Direction::BOTTOM;

    case Direction::BOTTOM:
      return Direction::TOP;

    case Direction::LEFT:
      return Direction::RIGHT;

    case Direction::RIGHT:
      return Direction::LEFT;

    case Direction::TOP_RIGHT:
      return Direction::BOTTOM_LEFT;

    case Direction::BOTTOM_LEFT:
      return Direction::TOP_RIGHT;
  }
  ASSERT_NOT_REACHED();
  return Direction::TOP;
}

inline std::pair<Direction, Direction> findOrthogonalDirections(
    Direction direction) {
  switch (direction) {
    case Direction::TOP_LEFT:
    case Direction::BOTTOM_RIGHT:
      return { Direction::TOP_RIGHT, Direction::BOTTOM_LEFT };

    case Direction::TOP:
    case Direction::BOTTOM:
      return { Direction::RIGHT, Direction::LEFT };

    case Direction::LEFT:
    case Direction::RIGHT:
      return { Direction::TOP, Direction::BOTTOM };

    case Direction::TOP_RIGHT:
    case Direction::BOTTOM_LEFT:
      return { Direction::TOP_LEFT, Direction::BOTTOM_RIGHT };
  }
  ASSERT_NOT_REACHED();
  return { Direction::TOP_LEFT, Direction::BOTTOM_RIGHT };
}

inline std::pair<size_t, size_t> getOffset(Direction direction) {
  switch (direction) {
    case Direction::TOP_LEFT:
      return { -1, -1 };

    case Direction::BOTTOM_RIGHT:
      return { 1, 1 };

    case Direction::TOP:
      return { 0, -1 };

    case Direction::BOTTOM:
      return { 0, 1 };

    case Direction::LEFT:
      return { -1, 0 };

    case Direction::RIGHT:
      return { 1, 0 };

    case Direction::TOP_RIGHT:
      return { 1, -1 };

    case Direction::BOTTOM_LEFT:
      return { -1, 1 };
  }
  ASSERT_NOT_REACHED();
  return { 0, 0 };
}

template <typename InputImage, typename OutputImage>
using ImageMapFunction = std::function<typename OutputImage::pixel_t::value_t(
    const typename InputImage::pixel_t&)>;

template <typename InputImage, typename OutputImage>
OutputImage mapImage(const InputImage& img,
                     ImageMapFunction<InputImage, OutputImage> mapFunction) {
  OutputImage ret(img.width(), img.height());

  for (unsigned y = 0; y < img.height(); ++y) {
    for (unsigned x = 0; x < img.width(); ++x) {
      ret[y][x].val(mapFunction(img[y][x]));
    }
  }
  return ret;
}
