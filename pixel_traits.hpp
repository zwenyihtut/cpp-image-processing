#pragma once
#include <cstdint>

template <unsigned Channels, typename ChannelType, unsigned ColorType>
struct PixelTraits {
  static constexpr unsigned CHANNELS = Channels;
  using channel_t = ChannelType;
  static constexpr unsigned COLOR_TYPE = ColorType;
};

using Rgb8PixelTraits = PixelTraits<3, uint8_t, PNG_COLOR_TYPE_RGB>;
using Rgb16PixelTraits = PixelTraits<3, uint16_t, PNG_COLOR_TYPE_RGB>;

using Rgba8PixelTraits = PixelTraits<4, uint8_t, PNG_COLOR_TYPE_RGBA>;
using Rgba16PixelTraits = PixelTraits<4, uint16_t, PNG_COLOR_TYPE_RGBA>;

using G8PixelTraits = PixelTraits<1, uint8_t, PNG_COLOR_TYPE_GRAY>;
using G16PixelTraits = PixelTraits<1, uint16_t, PNG_COLOR_TYPE_GRAY>;

using DoublePixelTraits = PixelTraits<1, double, 0>;
