#pragma once
#include <array>
#include <iostream>
#include <sstream>
#include <vector>

template <typename Element>
class MatView;

template <typename Element>
class Mat {
 public:
  friend class MatView<Element>;
  Mat(const std::vector<unsigned>& dimensions) : Mat(dimensions, {}) {}
  Mat(const std::vector<unsigned>& dimensions,
      const std::vector<Element>& elements)
      : mElements(elements), mDimensions(dimensions), mSize(1) {
    for (unsigned sz : mDimensions) {
      mSize *= sz;
    }
  }

  Element operator()(std::initializer_list<unsigned> indices) {
    assert(indices.size() == mDimensions.size());

  }
  Mat operator+(const Mat& other) {
    for (unsigned i = 0; i < mDimensions.size(); ++i) {
      assert(dimension(i) == other.dimension(i));
    }

    std::vector<Element> outputElements(size());

    for (unsigned i = 0; i < size(); ++i) {
      outputElements[i] = mElements[i] + other.mElements[i];
    }

    return Mat(mDimensions, outputElements);
  }
  
  unsigned dimensions() const { return mDimensions.size(); }
  Mat operator-(const Mat& other) {
    for (unsigned i = 0; i < mDimensions.size(); ++i) {
      assert(dimension(i) == other.dimension(i));
    }

    std::vector<Element> outputElements(size());

    for (unsigned i = 0; i < size(); ++i) {
      outputElements[i] = mElements[i] - other.mElements[i];
    }

    return Mat(mDimensions, outputElements);
  }

  MatView<Element> operator[](unsigned index);

  unsigned size() const { return mSize; }
  unsigned dimension(unsigned index) const { return mDimensions[index]; }

 private:
  std::vector<Element> mElements;
  const std::vector<unsigned> mDimensions;
  unsigned mSize;
};

template <typename E>
class MatViewBase {
 public:
  MatViewBase(Mat<E>& matrix,
              unsigned currentDimension,
              unsigned index,
              unsigned offset)
      : mMatrix(matrix),
        mCurrentDimension(currentDimension),
        mIndex(index),
        mOffset(offset) {}

 protected:
  Mat<E>& mMatrix;
  unsigned mCurrentDimension;
  unsigned mIndex;
  unsigned mOffset;
};

template <typename E>
class MatView : public MatViewBase<E> {
 public:
  using MatViewBase<E>::MatViewBase;

  MatView<E> operator[](unsigned index) {
    checkRange(index);

    unsigned stride = this->mMatrix.dimension(this->mCurrentDimension);
    return MatView<E>(this->mMatrix, this->mCurrentDimension - 1, index,
                      this->mOffset + this->mIndex * stride);
  };

  operator E() const {
    assert(this->mCurrentDimension == 0);
    return this->mMatrix.mElements.at(this->mOffset + this->mIndex);
  }

  MatView& operator=(const E& value) {
    assert(this->mCurrentDimension == 0);
    this->mMatrix.mElements.at(this->mOffset + this->mIndex) = value;
    return *this;
  }
  
 private:
  void checkRange(unsigned index) const {
    if (index >= this->mMatrix.dimension(this->mMatrix.mDimensions.size() -
                                         this->mCurrentDimension)) {
      std::ostringstream errStream;
      errStream << "Out of range access: index = " << index
                << ", dimension length = "
                << this->mMatrix.dimension(this->mMatrix.mDimensions.size() -
                                           this->mCurrentDimension);
      throw std::out_of_range(errStream.str());
    }
  }
};

template <typename E>
MatView<E> Mat<E>::operator[](unsigned index) {
  if (index > this->mDimensions[0]) {
    throw std::out_of_range("");
  }
  return MatView<E>(*this, this->mDimensions.size() - 1, index, 0);
};
