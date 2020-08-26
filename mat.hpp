#pragma once
#include <array>
#include <functional>
#include <numeric>
#include <sstream>
#include <vector>

template <typename Element>
class MatAccessor;
template <typename Element>
class ConstMatAccessor;


template <typename Element>
class Mat {
 public:
  template <typename E, typename M, typename D>
  friend class MatAccessorBase;
  friend class MatAccessor<Element>;
  friend class ConstMatAccessor<Element>;
  
  template <typename T>
  friend class Mat;

  using index_t = MatAccessor<Element>;
  using const_index_t = ConstMatAccessor<Element>;

  using iterator = typename std::vector<Element>::iterator;
  using const_iterator = typename std::vector<Element>::const_iterator;
  using size_type = typename std::vector<Element>::size_type;

  Mat(const std::vector<size_type>& dimensions);
  Mat(const std::vector<size_type>& dimensions,
      const std::vector<Element>& elements);

  Mat(const std::vector<size_type>& dimensions,
      const std::function<Element(unsigned)>& generator);

  iterator begin() { return this->mElements.begin(); }
  const_iterator cbegin() const { return this->mElements.cbegin(); }

  iterator end() { return this->mElements.end(); }
  const_iterator cend() const { return this->mElements.cend(); }

  Element operator()(std::initializer_list<unsigned> indices) const;

  Element operator()(unsigned index) const;
  void operator()(unsigned index, const Element& value);

  Mat& operator=(const std::initializer_list<Element>& other);
  Mat& operator+=(const Mat& other);
  Mat& operator+=(const std::initializer_list<Element>& other);

  Mat operator-(const Mat& other);
  Mat operator+(const Mat& other);

  unsigned dimensions() const;

  MatAccessor<Element> operator[](unsigned index);
  ConstMatAccessor<Element> operator[](unsigned index) const;

  size_type size() const;
  size_type dimension(unsigned index) const;
  
  template <typename T>
  Mat<T> clone() const {
    Mat<T> cpy(mDimensions, decltype(Mat<T>::mElements){});
    for (unsigned i = 0; i < mSize; ++i) {
      cpy.mElements.push_back(T(mElements[i]));
    }
    return cpy;
  }
 protected:
  std::vector<Element> mElements;
  std::vector<size_type> mDimensions;
  std::vector<unsigned> mOffsetMultipliers;
  unsigned mSize;
};

#include "mat_accessor.hpp"

template <typename E>
MatAccessor<E> Mat<E>::operator[](unsigned index) {
  if (index > this->mDimensions[0]) {
    throw std::out_of_range("");
  }
  return MatAccessor<E>(*this, 0, index, 0);
};

template <typename E>
ConstMatAccessor<E> Mat<E>::operator[](unsigned index) const {
  if (index > this->mDimensions[0]) {
    throw std::out_of_range("");
  }
  return ConstMatAccessor<E>(*this, 0, index, 0);
};

template <typename Element>
Mat<Element>::Mat(const std::vector<size_type>& dimensions)
    : Mat(dimensions, decltype(mElements)()) {
  this->mElements = std::vector<Element>(this->size(), 0);
}

template <typename Element>
Mat<Element>::Mat(const std::vector<size_type>& dimensions,
                  const std::vector<Element>& elements)
    : mElements(elements),
      mDimensions(dimensions),
      mOffsetMultipliers(dimensions.size()),
      mSize(1) {
  for (unsigned sz : mDimensions) {
    mSize *= sz;
  }
  mOffsetMultipliers.back() = 1;
  std::inclusive_scan(mDimensions.rbegin(), mDimensions.rend() - 1,
                      mOffsetMultipliers.rbegin() + 1, std::multiplies<int>());
}

template <typename Element>
Mat<Element>::Mat(const std::vector<size_type>& dimensions,
                  const std::function<Element(unsigned)>& generator)
    : Mat(dimensions, decltype(mElements)()) {
  for (unsigned i = 0; i < mSize; ++i) {
    mElements.push_back(generator(i));
  }
}

template <typename Element>
Element Mat<Element>::operator()(
    std::initializer_list<unsigned> indices) const {
  assert(indices.size() == mDimensions.size());
  unsigned index = 0;
  unsigned i = 0;
  for (const unsigned v : indices) {
    index += mDimensions[i] * v;
    ++i;
  }
  return mElements[index];
}

template <typename Element>
Element Mat<Element>::operator()(unsigned index) const {
  return mElements[index];
}

template <typename Element>
void Mat<Element>::operator()(unsigned index, const Element& value) {
  mElements[index] = value;
}

template <typename Element>
Mat<Element>& Mat<Element>::operator=(
    const std::initializer_list<Element>& other) {
  assert(mElements.size() == other.size());
  unsigned i = 0;
  for (const auto& v : other) {
    mElements[i] = v;
    ++i;
  }
  return *this;
}

template <typename Element>
Mat<Element>& Mat<Element>::operator+=(const Mat& other) {
  for (unsigned i = 0; i < mDimensions.size(); ++i) {
    assert(dimension(i) == other.dimension(i));
  }
  for (unsigned i = 0; i < mElements.size(); ++i) {
    mElements[i] += other.mElements[i];
  }
  return *this;
}

template <typename Element>
Mat<Element>& Mat<Element>::operator+=(
    const std::initializer_list<Element>& other) {
  assert(mElements.size() == other.size());
  unsigned i = 0;
  for (const auto& v : other) {
    mElements[i] += v;
    ++i;
  }
  return *this;
}

template <typename Element>
Mat<Element> Mat<Element>::operator+(const Mat& other) {
  for (unsigned i = 0; i < mDimensions.size(); ++i) {
    assert(dimension(i) == other.dimension(i));
  }

  std::vector<Element> outputElements(size());

  for (unsigned i = 0; i < size(); ++i) {
    outputElements[i] = mElements[i] + other.mElements[i];
  }

  return Mat(mDimensions, outputElements);
}

template <typename Element>
unsigned Mat<Element>::dimensions() const {
  return mDimensions.size();
}

template <typename Element>
Mat<Element> Mat<Element>::operator-(const Mat& other) {
  for (unsigned i = 0; i < mDimensions.size(); ++i) {
    assert(dimension(i) == other.dimension(i));
  }

  std::vector<Element> outputElements(size());

  for (unsigned i = 0; i < size(); ++i) {
    outputElements[i] = mElements[i] - other.mElements[i];
  }

  return Mat(mDimensions, outputElements);
}

template <typename Element>
typename Mat<Element>::size_type Mat<Element>::size() const {
  return mSize;
}

template <typename Element>
typename Mat<Element>::size_type Mat<Element>::dimension(unsigned index) const {
  return mDimensions[index];
}
