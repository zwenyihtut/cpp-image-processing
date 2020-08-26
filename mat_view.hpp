#pragma once
#include "mat.hpp"

template <typename T, typename E, typename I, typename CI>
class MatView {
 public:
  using element_t = E;

  MatView() = default;
  MatView(Mat<element_t>& mat) : mMat(&mat) {}
  MatView(MatView& other) : mMat(other.mMat) {}
  MatView(MatView&& other) : mMat(other.mMat) {}
  MatView& operator=(MatView& other) {
    mMat = other.mMat;
    return *this;
  }

  MatView& operator=(MatView&& other) {
    mMat = other.mMat;
    return *this;
  }

  unsigned dimensions() const {
    return static_cast<T*>(this)->dimensions();
  }
  
  unsigned size() const {
    return static_cast<T*>(this)->size();
  }

  I operator[](unsigned index) {
    return static_cast<T*>(this)->operator[](index);
  }

  CI operator[](unsigned index) const {
    return static_cast<T*>(this)->operator[](index);
  }

 protected:
  Mat<element_t>* mMat = nullptr;
};
