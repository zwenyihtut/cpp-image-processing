#pragma once
extern "C" {
#include <png.h>
}
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "libpng_wrapper.hpp"
#include "pixel_traits.hpp"
#include "pixels.hpp"

template <typename Derived, typename BufferType>
class ImageBuffer {
 public:
  BufferType& buffer() const {
    return static_cast<const Derived*>(this)->buffer();
  }

  size_t width() const { return static_cast<const Derived*>(this)->width(); }
  size_t height() const { return static_cast<const Derived*>(this)->height(); }
};

class ImageFileBuffer : public ImageBuffer<ImageFileBuffer, png_bytepp> {
  static std::string toColorTypeString(uint8_t colorType);

 public:
  using buffer_type = png_bytepp;

  ImageFileBuffer(const std::string& name);
  ImageFileBuffer(size_t width, size_t height, uint8_t colorType);

  void load(const std::string& name);
  void save(const std::string& filename);

  unsigned width() const;
  unsigned height() const;

  uint8_t bitDepth() const;
  std::string colorType() const;

  png_bytepp& buffer() { return mBuffer; }
  const png_bytepp& buffer() const { return mBuffer; }

  ~ImageFileBuffer();

 private:
  png_structp mPngPtr = nullptr;
  png_infop mInfoPtr = nullptr;
  png_bytepp mBuffer = nullptr;

  bool mWriteReady = false;
  bool mBufferIsSelfManaged = false;
};

class DoubleBuffer {
 public:
  using buffer_type = std::vector<std::vector<double>>;
  DoubleBuffer(size_t width, size_t height, unsigned unused)
      : mBuffer(height, std::vector<double>(width, 0)) {}

  unsigned height() const { return mBuffer.size(); }
  unsigned width() const { return mBuffer[0].size(); }

  uint8_t bitDepth() const { return sizeof(double) * CHAR_BIT; }

  auto& buffer() { return mBuffer; }
  const auto& buffer() const { return mBuffer; }

 private:
  std::vector<std::vector<double>> mBuffer;
};

template <typename Element, unsigned ElementPerPixel>
class Buffer {
 public:
  using buffer_type = std::vector<std::vector<Element>>;
  using pixel_trait_t = PixelTraits<ElementPerPixel, Element, 0>;
  using pixel_t = PixelBase<pixel_trait_t, buffer_type>;

  Buffer(size_t width, size_t height, unsigned unused)
      : mBuffer(height, std::vector<Element>(width, 0)) {}

  unsigned height() const { return mBuffer.size(); }
  unsigned width() const { return mBuffer.empty() ? 0 : mBuffer[0].size(); }

  uint8_t bitDepth() const { return sizeof(double) * CHAR_BIT; }

  auto& buffer() { return mBuffer; }
  const auto& buffer() const { return mBuffer; }

 private:
  buffer_type mBuffer;
};
