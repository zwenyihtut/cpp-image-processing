#pragma once
#include <iterator>

template <typename Image>
class PixelIterator {
  using pixel_t = typename Image::pixel_t;
  using coord_t = std::pair<uint32_t, uint32_t>;
  using row_t = typename Image::row_t;

 public:
  using value_type = std::pair<pixel_t, coord_t>;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using pointer = value_type*;
  using iterator_category = std::forward_iterator_tag;

  PixelIterator(Image* img, unsigned x, unsigned y, bool isEnd)
      : mRowCache(), mImg(img), mX(x), mY(y), mIsEnd(isEnd) {
    if (mImg)
      mRowCache = (*mImg)[0];
  }

  const value_type operator*() const {
    auto pixel = mRowCache[mX];
    coord_t coords = {mX, mY};
    return {pixel, coords};
  }

  value_type operator*() {
    auto pixel = mRowCache[mX];
    coord_t coords = {mX, mY};
    return {pixel, coords};
  }

  bool operator==(const PixelIterator& other) const {
    if (this->mIsEnd && other.mIsEnd)
      return true;
    return mImg == other.mImg && mX == other.mX && mY == other.mY;
  }

  bool operator!=(const PixelIterator& other) const {
    return !(*this == other);
  }

  PixelIterator& operator++() {
    ++mX;
    if (mX == mImg->width()) {
      mX = 0;
      ++mY;
      mRowCache = (*mImg)[mY];
    }
    if (mY == mImg->height()) {
      mIsEnd = true;
    }
    return *this;
  }

 private:
  row_t mRowCache;
  Image* mImg;
  unsigned mX, mY;
  bool mIsEnd = false;
};

template <typename Image>
struct ImageTraverser {
  using iterator = PixelIterator<Image>;
  using const_iterator = PixelIterator<const Image>;
  using value_type = const PixelIterator<Image>;

  ImageTraverser(Image& img) : mImg(&img) {}

  iterator begin() { return PixelIterator<Image>(mImg, 0, 0, false); }
  const_iterator cbegin() const {
    return PixelIterator<Image>(mImg, 0, 0, false);
  }

  iterator end() { return PixelIterator<Image>(nullptr, 0, 0, true); }
  const_iterator cend() const {
    return PixelIterator<Image>(nullptr, 0, 0, true);
  }

 private:
  Image* mImg;
};

template <typename Image>
ImageTraverser<Image> pixels(Image& img) {
  return ImageTraverser<Image>(img);
}
