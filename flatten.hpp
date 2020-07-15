#pragma once
#include <array>
#include <type_traits>

template <typename NestedArray> class RowFirstIterator {
public:
  using RowIterType = decltype(std::begin(std::declval<NestedArray&>()));
  using ColIterType = decltype(std::begin(*std::declval<RowIterType&>()));

  using ValueType = decltype(*(std::declval<ColIterType&>()));

  RowFirstIterator(NestedArray *arr, ColIterType iter)
      : mArr(arr), mIter(iter) {}

  RowFirstIterator &operator++() {
    if (!mArr) return *this;
    ++(*mIter);
    // using `while` instead of `if` to skip over empty rows
    while (*mIter == std::end((*mArr)[currRow])) {
      ++currRow;
      if (currRow == std::size(*mArr)) return *this;
      mIter = std::begin((*mArr)[currRow]);
    }
    return *this;
  }

  bool operator==(const RowFirstIterator &other) {
    if (!mArr && !other.mArr) return true;
    return mIter == other.mIter;
  }
  
  ValueType operator*() {
    return *(*mIter);
  }

  bool operator!=(const RowFirstIterator &other) { return !(*this == other); }

private:
  NestedArray *mArr;
  std::optional<ColIterType> mIter;
  size_t currRow = 0;
};

template <typename NestedArray> struct RowFirstIterGenerator {
  RowFirstIterGenerator(NestedArray &arr) : arr(&arr) {}

  RowFirstIterator<NestedArray> begin() {
    if (std::begin(*arr) == std::end(*arr)) {
        return RowFirstIterator<NestedArray>(nullptr, {});
    }

    return RowFirstIterator<NestedArray>(arr, std::begin((*arr)[0]));
  };

  RowFirstIterator<NestedArray> end() {
    if (std::begin(*arr) == std::end(*arr)) {
        return RowFirstIterator<NestedArray>(nullptr, {});
    }
    const auto sz = std::size(*arr);
    return RowFirstIterator<NestedArray>(arr, std::end((*arr)[sz-1]));
  }
  NestedArray *arr;
};

template <typename NestedArray>
RowFirstIterGenerator<NestedArray> flattened(NestedArray &arr) {
  return RowFirstIterGenerator<NestedArray>(arr);
}
