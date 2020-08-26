#pragma once
#include "mat.hpp"
#include "mat_view.hpp"

template <typename E>
class MatView2DAccessor {
 public:
  MatView2DAccessor(Mat<E>& mat, unsigned x, unsigned y)
      : mX(x), mY(y), mMat(mat) {}
  MatView2DAccessor& operator=(const E& value) {
    this->mMat[mY][mX][0] = value;
  }

  MatView2DAccessor operator[](unsigned x) const {
    return MatView2DAccessor(mMat, x, mY);
  }

  operator E() const { return this->mMat[mY][mX][0]; }
  
 private:
  unsigned mX = 0;
  unsigned mY = 0;
  Mat<E>& mMat;
};

template <typename E>
class ConstMatView2DAccessor {
 public:
  ConstMatView2DAccessor(const Mat<E>& mat, unsigned x, unsigned y)
      : mX(x), mY(y), mMat(mat) {}

  ConstMatView2DAccessor operator[](unsigned x) const {
    return ConstMatView2DAccessor(mMat, x, mY);
  }
  operator E() const { return this->mMat[mY][mX][0]; }

 private:
  unsigned mX = 0;
  unsigned mY = 0;
  const Mat<E>& mMat;
};

template <typename E>
class MatView2D : public MatView<MatView2D<E>,
                                   E,
                                   MatView2DAccessor<E>,
                                   ConstMatView2DAccessor<E>> {
 public:
  using MatView<MatView2D<E>,
                E,
                MatView2DAccessor<E>,
                ConstMatView2DAccessor<E>>::MatView;

  unsigned dimensions() const { return 2; }
  unsigned size() const { return this->mMat->size(); }

  MatView2DAccessor<E> operator[](unsigned index) {
    return MatView2DAccessor(*this->mMat, 0, index);
  }
  ConstMatView2DAccessor<E> operator[](unsigned index) const {
    return ConstMatView2DAccessor(*this->mMat, 0, index);
  }

  unsigned height() const { return this->mMat->dimension(0); }
  unsigned width() const { return this->mMat->dimension(1); }

};
