#pragma once
#include <array>
#include <iostream>
#include <sstream>
#include <vector>

template <typename Element> class MatView;
template <typename Element> class ConstMatView;

template <typename Element> class Mat {
public:
  friend class MatView<Element>;
  friend class ConstMatView<Element>;

  Mat(const std::vector<unsigned> &dimensions) : Mat(dimensions, {}) {}
  Mat(const std::vector<unsigned> &dimensions,
      const std::vector<Element> &elements)
      : mElements(elements), mDimensions(dimensions), mSize(1) {
    for (unsigned sz : mDimensions) {
      mSize *= sz;
    }
  }
  
  Element operator()(std::initializer_list<unsigned> indices) const {
    assert(indices.size() == mDimensions.size());
    unsigned index = 0;
    unsigned i = 0;
    for (const unsigned v : indices) {
      index += mDimensions[i] * v;
      ++i;
    }
    return mElements[index];
  }
  
  Element operator()(unsigned index) const {
    return mElements[index];
  }

  void operator()(unsigned index, const Element& value) {
    mElements[index] = value;
  }

  Mat &operator=(const std::initializer_list<Element> &other) {
    assert(mElements.size() == other.size());
    unsigned i = 0;
    for (const auto &v : other) {
      mElements[i] = v;
      ++i;
    }
    return *this;
  }

  Mat &operator+=(const Mat &other) {
    for (unsigned i = 0; i < mDimensions.size(); ++i) {
      assert(dimension(i) == other.dimension(i));
    }
    for (unsigned i = 0; i < mElements.size(); ++i) {
      mElements[i] += other.mElements[i];
    }
    return *this;
  }

  Mat &operator+=(const std::initializer_list<Element> &other) {
    assert(mElements.size() == other.size());
    unsigned i = 0;
    for (const auto &v : other) {
      mElements[i] += v;
      ++i;
    }
    return *this;
  }

  Mat operator+(const Mat &other) {
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
  Mat operator-(const Mat &other) {
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
  ConstMatView<Element> operator[](unsigned index) const;

  unsigned size() const { return mSize; }
  unsigned dimension(unsigned index) const { return mDimensions[index]; }

private:
  std::vector<Element> mElements;
  const std::vector<unsigned> mDimensions;
  unsigned mSize;
};

template <typename E> class MatViewBase {
public:
  MatViewBase(Mat<E> &matrix, unsigned currentDimension, unsigned index,
              unsigned offset)
      : mMatrix(matrix), mCurrentDimension(currentDimension), mIndex(index),
        mOffset(offset) {}

protected:
  Mat<E> &mMatrix;
  unsigned mCurrentDimension;
  unsigned mIndex;
  unsigned mOffset;
};

template <typename E> class MatView : public MatViewBase<E> {
public:
  using MatViewBase<E>::MatViewBase;

  MatView<E> operator[](unsigned index) {
    checkRange(index);

    unsigned stride = this->mMatrix.dimension(this->mCurrentDimension);
    return MatView<E>(this->mMatrix, this->mCurrentDimension - 1, index,
                      this->mOffset + this->mIndex * stride);
  };

  const MatView<E> operator[](unsigned index) const {
    checkRange(index);

    unsigned stride = this->mMatrix.dimension(this->mCurrentDimension);
    return MatView<E>(this->mMatrix, this->mCurrentDimension - 1, index,
                      this->mOffset + this->mIndex * stride);
  };

  operator E() const {
    assert(this->mCurrentDimension == 0);
    return this->mMatrix.mElements.at(this->mOffset + this->mIndex);
  }

  MatView &operator=(const E &value) {
    assert(this->mCurrentDimension == 0);
    const auto i = this->mOffset + this->mIndex;
    std::cout << i << ' ';
    this->mMatrix.mElements.at(i) = value;
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

template <typename E> class ConstMatViewBase {
public:
  ConstMatViewBase(const Mat<E> &matrix, unsigned currentDimension,
                   unsigned index, unsigned offset)
      : mMatrix(matrix), mCurrentDimension(currentDimension), mIndex(index),
        mOffset(offset) {}

protected:
  const Mat<E> &mMatrix;
  unsigned mCurrentDimension;
  unsigned mIndex;
  unsigned mOffset;
};

template <typename E> class ConstMatView : public ConstMatViewBase<E> {
public:
  using ConstMatViewBase<E>::ConstMatViewBase;

  ConstMatView<E> operator[](unsigned index) const {
    checkRange(index);

    unsigned stride = this->mMatrix.dimension(this->mCurrentDimension);
    return ConstMatView<E>(this->mMatrix, this->mCurrentDimension - 1, index,
                           this->mOffset + this->mIndex * stride);
  };

  operator E() const {
    assert(this->mCurrentDimension == 0);
    return this->mMatrix.mElements.at(this->mOffset + this->mIndex);
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

template <typename E> MatView<E> Mat<E>::operator[](unsigned index) {
  if (index > this->mDimensions[0]) {
    throw std::out_of_range("");
  }
  return MatView<E>(*this, this->mDimensions.size() - 1, index, 0);
};

template <typename E> ConstMatView<E> Mat<E>::operator[](unsigned index) const {
  if (index > this->mDimensions[0]) {
    throw std::out_of_range("");
  }
  return ConstMatView<E>(*this, this->mDimensions.size() - 1, index, 0);
};
