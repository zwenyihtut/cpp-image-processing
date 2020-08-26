#pragma once
#include "mat.hpp"
#include <cassert>

template <typename E, typename M, typename D>
class MatAccessorBase {
 public:
  MatAccessorBase(M& matrix,
              unsigned currentDimension,
              unsigned index,
              unsigned offset)
      : mMatrix(matrix),
        mCurrentDimension(currentDimension),
        mIndex(index),
        mOffset(offset) {}

  D operator[](unsigned index);
  const D operator[](unsigned index) const;
  operator E() const;

 protected:
  void assertRange(unsigned index) const;

  M& mMatrix;
  unsigned mCurrentDimension;
  unsigned mIndex;
  unsigned mOffset;
};

template <typename E>
class MatAccessor : public MatAccessorBase<E, Mat<E>, MatAccessor<E>> {
 public:
  using MatAccessorBase<E, Mat<E>, MatAccessor<E>>::MatAccessorBase;
  MatAccessor& operator=(const E& value);

};

template <typename E>
class ConstMatAccessor : public MatAccessorBase<E, const Mat<E>, ConstMatAccessor<E>> {
 public:
  using MatAccessorBase<E, const Mat<E>, ConstMatAccessor<E>>::MatAccessorBase;
};

template <typename E, typename M, typename D>
D MatAccessorBase<E, M, D>::operator[](unsigned index) {
#ifndef NDEBUG
  assertRange(index);
#endif
  unsigned stride = this->mMatrix.mOffsetMultipliers[this->mCurrentDimension];
  return D(this->mMatrix, this->mCurrentDimension + 1, index,
                    this->mOffset + this->mIndex * stride);
};

template <typename E, typename M, typename D>
const D MatAccessorBase<E, M, D>::operator[](unsigned index) const {
#ifndef NDEBUG
  assertRange(index);
#endif

  unsigned stride = this->mMatrix.mOffsetMultipliers[this->mCurrentDimension];
  return D(this->mMatrix, this->mCurrentDimension + 1, index,
                    this->mOffset + this->mIndex * stride);
};

template <typename E, typename M, typename D>
MatAccessorBase<E, M, D>::operator E() const {
  assert(this->mCurrentDimension == this->mMatrix.mDimensions.size() - 1);
  return this->mMatrix.mElements.at(this->mOffset + this->mIndex);
}

template <typename E>
MatAccessor<E>& MatAccessor<E>::operator=(const E& value) {
  assert(this->mCurrentDimension == this->mMatrix.mDimensions.size() - 1);
  const auto i = this->mOffset + this->mIndex;
  this->mMatrix.mElements.at(i) = value;
  return *this;
}

template <typename E, typename M, typename D>
void MatAccessorBase<E, M, D>::assertRange(unsigned index) const {
  if (index >= this->mMatrix.dimension(this->mCurrentDimension + 1)) {
    std::ostringstream errText;
    errText << "Out of range access: index = " << index
            << ", dimension length = "
            << this->mMatrix.dimension(this->mCurrentDimension + 1)
            << ", current dimension = " << this->mCurrentDimension;
    throw std::out_of_range(errText.str());
  }
}


