#pragma once
extern "C" {
#include <png.h>
}
#include <array>
#include <cassert>
#include <tuple>
#include "pixel_traits.hpp"

using DoublePixelTraits = PixelTraits<1, double, PNG_COLOR_TYPE_GRAY>;

template <typename PixelTraits, typename Buffer = png_bytepp>
class PixelBase {
 public:
  static constexpr auto CHANNELS = PixelTraits::CHANNELS;
  static constexpr auto COLOR_TYPE = PixelTraits::COLOR_TYPE;

  using traits = PixelTraits;
  using channel_t = typename traits::channel_t;
  using value_t = std::array<channel_t, CHANNELS>;

  PixelBase(Buffer& rows, size_t x, size_t y) : mRows(rows), mX(x), mY(y) {}

  const value_t val() const {
    value_t ret;
    for (unsigned i = 0; i < CHANNELS; ++i) {
      ret[i] = get(i);
    }
    return ret;
  }

  void val(value_t values) {
    for (unsigned i = 0; i < CHANNELS; ++i) {
      mRows[mY][CHANNELS * mX + i] = values[i];
    }
  }

  typename traits::channel_t get(const size_t index) const {
    return mRows[mY][CHANNELS * mX + index];
  }

  void set(const size_t index, typename traits::channel_t value) {
    mRows[mY][CHANNELS * mX + index] = value;
  }

 protected:
  Buffer& mRows;
  size_t mX;
  size_t mY;
};

class PixelRGB8 : public PixelBase<Rgb8PixelTraits> {
 public:
  using PixelBase::PixelBase;

  uint8_t red() const { return mRows[mY][3 * mX]; }
  uint8_t green() const { return mRows[mY][3 * mX + 1]; }
  uint8_t blue() const { return mRows[mY][3 * mX + 2]; }

  void red(uint8_t val) { mRows[mY][3 * mX] = val; }
  void green(uint8_t val) { mRows[mY][3 * mX + 1] = val; }
  void blue(uint8_t val) { mRows[mY][3 * mX + 2] = val; }
};

class PixelRGBA8 : public PixelBase<Rgba8PixelTraits> {
 public:
  using PixelBase::PixelBase;

  uint8_t red() const { return mRows[mY][4 * mX]; }
  uint8_t green() const { return mRows[mY][4 * mX + 1]; }
  uint8_t blue() const { return mRows[mY][4 * mX + 2]; }
  uint8_t alpha() const { return mRows[mY][4 * mX + 3]; }

  void red(uint8_t val) { mRows[mY][4 * mX] = val; }
  void green(uint8_t val) { mRows[mY][4 * mX + 1] = val; }
  void blue(uint8_t val) { mRows[mY][4 * mX + 2] = val; }
  void alpha(uint8_t val) { mRows[mY][4 * mX + 3] = val; }
};

class PixelG8 : public PixelBase<G8PixelTraits> {
 public:
  using PixelBase::PixelBase;
};

class PixelDouble : public PixelBase<DoublePixelTraits> {
 public:
  using PixelBase::PixelBase;
};

class DoublePixel
    : public PixelBase<DoublePixelTraits, std::vector<std::vector<double>>> {
 public:
  using value_t = double;
  using PixelBase::PixelBase;

  const value_t val() const { return mRows[mY][mX]; }

  void val(value_t value) { mRows[mY][mX] = value; }
};
