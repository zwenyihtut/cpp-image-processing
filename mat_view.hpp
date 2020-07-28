#pragma once
#include "mat.hpp"
#include <cassert>

template <typename E, typename M, typename D>
class MatViewBase {
 public:
  MatViewBase(M& matrix,
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
class MatView : public MatViewBase<E, Mat<E>, MatView<E>> {
 public:
  using MatViewBase<E, Mat<E>, MatView<E>>::MatViewBase;
  MatView& operator=(const E& value);

};

template <typename E>
class ConstMatView : public MatViewBase<E, const Mat<E>, ConstMatView<E>> {
 public:
  using MatViewBase<E, const Mat<E>, ConstMatView<E>>::MatViewBase;
};

template <typename E, typename M, typename D>
D MatViewBase<E, M, D>::operator[](unsigned index) {
#ifndef NDEBUG
  assertRange(index);
#endif
  unsigned stride = this->mMatrix.mOffsetMultipliers[this->mCurrentDimension];
  return D(this->mMatrix, this->mCurrentDimension + 1, index,
                    this->mOffset + this->mIndex * stride);
};

template <typename E, typename M, typename D>
const D MatViewBase<E, M, D>::operator[](unsigned index) const {
#ifndef NDEBUG
  assertRange(index);
#endif

  unsigned stride = this->mMatrix.mOffsetMultipliers[this->mCurrentDimension];
  return D(this->mMatrix, this->mCurrentDimension + 1, index,
                    this->mOffset + this->mIndex * stride);
};

template <typename E, typename M, typename D>
MatViewBase<E, M, D>::operator E() const {
  assert(this->mCurrentDimension == this->mMatrix.mDimensions.size() - 1);
  return this->mMatrix.mElements.at(this->mOffset + this->mIndex);
}

template <typename E>
MatView<E>& MatView<E>::operator=(const E& value) {
  assert(this->mCurrentDimension == this->mMatrix.mDimensions.size() - 1);
  const auto i = this->mOffset + this->mIndex;
  this->mMatrix.mElements.at(i) = value;
  return *this;
}

template <typename E, typename M, typename D>
void MatViewBase<E, M, D>::assertRange(unsigned index) const {
  if (index >= this->mMatrix.dimension(this->mCurrentDimension + 1)) {
    std::ostringstream errText;
    errText << "Out of range access: index = " << index
            << ", dimension length = "
            << this->mMatrix.dimension(this->mCurrentDimension + 1)
            << ", current dimension = " << this->mCurrentDimension;
    throw std::out_of_range(errText.str());
  }
}


