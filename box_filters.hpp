#pragma once
#include <cstdint>
#include <vector>

class Box {
 public:
  struct Coord {
    unsigned x0, y0, x1, y1;
  };

  Box(unsigned x0, unsigned y0, unsigned x1, unsigned y1, int8_t value)
      : mCoord({x0, y0, x1, y1}), mVal(value) {}

  Coord coordinates() const { return mCoord; }
  int8_t val() const { return mVal; }

  void transpose() {
    std::swap(mCoord.x0, mCoord.y0);
    std::swap(mCoord.x1, mCoord.y1);
  }
  unsigned area() const {
    return (mCoord.x1 - mCoord.x0 + 1) * (mCoord.y1 - mCoord.y0 + 1);
  }

 private:
  Coord mCoord;
  int8_t mVal;
};

class BoxFilter {
 public:
  void addBox(const Box& box) { mBoxes.push_back(box); }

  typename std::vector<Box>::iterator begin() { return mBoxes.begin(); }
  typename std::vector<Box>::iterator end() { return mBoxes.end(); }

  typename std::vector<Box>::const_iterator begin() const {
    return mBoxes.begin();
  }
  typename std::vector<Box>::const_iterator end() const { return mBoxes.end(); }

  typename std::vector<Box>::const_iterator cbegin() const {
    return mBoxes.cbegin();
  }
  typename std::vector<Box>::const_iterator cend() const {
    return mBoxes.cend();
  }

  void transpose() {
    for (auto& box : mBoxes) {
      box.transpose();
    }
  }

 private:
  std::vector<Box> mBoxes;
};
