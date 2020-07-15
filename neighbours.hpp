#pragma once
#include "utility.hpp"

class NeighbourIterator {
 public:
  NeighbourIterator(int x, int y, bool isEnd)
      : x(x), y(y), mEnd(isEnd), mDir(Direction::TOP_LEFT) {}

  NeighbourIterator& operator++() {
    switch (mDir) {
      case Direction::TOP_LEFT:
        mDir = Direction::TOP;
        break;
      case Direction::TOP:
        mDir = Direction::TOP_RIGHT;
        break;
      case Direction::TOP_RIGHT:
        mDir = Direction::RIGHT;
        break;
      case Direction::RIGHT:
        mDir = Direction::BOTTOM_RIGHT;
        break;
      case Direction::BOTTOM_RIGHT:
        mDir = Direction::BOTTOM;
        break;
      case Direction::BOTTOM:
        mDir = Direction::BOTTOM_LEFT;
        break;
      case Direction::BOTTOM_LEFT:
        mDir = Direction::LEFT;
        break;
      case Direction::LEFT:
        mEnd = true;
        break;
    }
    return *this;
  }

  std::pair<int, int> operator*() const {
    const auto [ox, oy] = getOffset(mDir);
    return { x + (int)ox, y + (int)oy };
  }
  bool operator==(const NeighbourIterator& other) const {
    if (this->mEnd && other.mEnd)
      return true;
    if (this->mEnd || other.mEnd)
      return false;
    return this->mDir == other.mDir && this->x == other.x && this->y == other.y;
  }

  bool operator!=(const NeighbourIterator& other) const {
    return !(*this == other);
  }

 private:
  Direction mDir;
  int x, y;
  bool mEnd;
};

class Neighbours {
 public:
  Neighbours(int x, int y) : x(x), y(y) {}

  NeighbourIterator begin() { return NeighbourIterator(x, y, false); }
  NeighbourIterator end() { return NeighbourIterator(x, y, true); }

 private:
  int x, y;
};

inline Neighbours neighbours(int x, int y) {
  return { x, y };
}
