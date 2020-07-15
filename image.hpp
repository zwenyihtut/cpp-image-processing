#pragma once

extern "C" {
#include <png.h>
}
#include <iostream>
#include <memory>
#include <sstream>
#include "image_file_buffer.hpp"

template <typename Pixel, typename BufferHolder>
struct ImageRow {
  using buffer_type = typename BufferHolder::buffer_type;

  ImageRow(buffer_type& rows, size_t rowIndex, size_t rowWidth)
      : mRows(&rows), mRowIndex(rowIndex), mSize(rowWidth) {}
  ImageRow() : mRows(nullptr), mRowIndex(0), mSize(0) {}

  const Pixel operator[](size_t colIndex) const {
    return Pixel(*mRows, colIndex, mRowIndex);
  }

  Pixel operator[](size_t colIndex) {
    return Pixel(*mRows, colIndex, mRowIndex);
  }

  buffer_type* mRows;
  size_t mRowIndex;
  size_t mSize;
};

template <typename Pixel, typename BufferHolder = ImageFileBuffer>
class Image {
 public:
  using row_t = ImageRow<Pixel, BufferHolder>;
  using pixel_t = Pixel;
  using buffer_type = typename BufferHolder::buffer_type;
  using buffer_holder_type = BufferHolder;

  Image() = default;

  Image(Image& img);
  Image(Image&& img);

  auto& operator=(const Image& other);
  auto& operator=(Image&& other);

  Image(buffer_holder_type& buffer);
  Image(size_t width, size_t height);

  std::string info() const;

  const ImageRow<pixel_t, buffer_holder_type> operator[](size_t rowIndex) const;
  ImageRow<pixel_t, buffer_holder_type> operator[](size_t rowIndex);

  size_t width() const;
  size_t height() const;

  buffer_holder_type* buffer();
  const buffer_holder_type* buffer() const;

  Image clone() const;

  ~Image();

 private:
  buffer_holder_type* mBuffer;
  bool mOwnsFile;
};

template <typename Pixel, typename BufferHolder>
Image<Pixel, BufferHolder>::Image(BufferHolder& buffer)
    : mBuffer(&buffer), mOwnsFile(false) {}

template <typename Pixel, typename BufferHolder>
Image<Pixel, BufferHolder>::Image(Image& img)
    : mBuffer(img.mBuffer), mOwnsFile(false) {}

template <typename Pixel, typename BufferHolder>
Image<Pixel, BufferHolder>::Image(size_t width, size_t height)
    : mBuffer(new BufferHolder(width, height, Pixel::COLOR_TYPE)),
      mOwnsFile(true) {}

template <typename Pixel, typename BufferHolder>
Image<Pixel, BufferHolder>::Image(Image&& img)
    : mBuffer(std::move(img.mBuffer)), mOwnsFile(img.mOwnsFile) {
  img.mOwnsFile = false;
}

template <typename Pixel, typename BufferHolder>
auto& Image<Pixel, BufferHolder>::operator=(const Image& other) {
  if (mOwnsFile)
    delete mBuffer;

  this->mBuffer = other.mBuffer;
  mOwnsFile = false;
  return *this;
}

template <typename Pixel, typename BufferHolder>
auto& Image<Pixel, BufferHolder>::operator=(Image&& other) {
  if (mOwnsFile)
    delete mBuffer;

  mBuffer = other.mBuffer;
  mOwnsFile = other.mOwnsFile;

  other.mOwnsFile = false;
  return *this;
}

template <typename Pixel, typename BufferHolder>
std::string Image<Pixel, BufferHolder>::info() const {
  std::ostringstream oss;
  oss << "Width: " << this->width() << ", Height: " << this->height();
  return oss.str();
}

template <typename Pixel, typename BufferHolder>
const ImageRow<Pixel, BufferHolder> Image<Pixel, BufferHolder>::operator[](
    size_t rowIndex) const {
  return { mBuffer->buffer(), rowIndex, this->width() };
}

template <typename Pixel, typename BufferHolder>
ImageRow<Pixel, BufferHolder> Image<Pixel, BufferHolder>::operator[](
    size_t rowIndex) {
  return { mBuffer->buffer(), rowIndex, this->width() };
}

template <typename Pixel, typename BufferHolder>
size_t Image<Pixel, BufferHolder>::width() const {
  return mBuffer->width();
}

template <typename Pixel, typename BufferHolder>
size_t Image<Pixel, BufferHolder>::height() const {
  return mBuffer->height();
}

template <typename Pixel, typename BufferHolder>
BufferHolder* Image<Pixel, BufferHolder>::buffer() {
  return mBuffer;
}

template <typename Pixel, typename BufferHolder>
const BufferHolder* Image<Pixel, BufferHolder>::buffer() const {
  return mBuffer;
}

template <typename Pixel, typename BufferHolder>
Image<Pixel, BufferHolder>::~Image() {
  if (mOwnsFile)
    delete mBuffer;
}

template <typename Pixel, typename BufferHolder>
Image<Pixel, BufferHolder> Image<Pixel, BufferHolder>::clone() const {
  Image ret(this->width(), this->height());
  for (size_t y = 0; y < height(); ++y) {
    for (size_t x = 0; x < width(); ++x) {
      ret[y][x].val((*this)[y][x].val());
    }
  }
  return ret;
}
