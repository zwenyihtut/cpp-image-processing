#pragma once

template <unsigned ROWS, unsigned COLS>
using Matrix = std::array<std::array<double, COLS>, ROWS>;

template <long unsigned N>
using matrix_t = Matrix<N, N>;

template <unsigned ROW, unsigned COL, typename Function, typename Matrix>
constexpr void fillCols(Function generator, Matrix& matrix) {
  matrix[ROW][COL] = generator(ROW, COL);

  if constexpr (COL == 0) {
    return;
  } else {
    fillCols<ROW, COL - 1>(generator, matrix);
  }
}

template <unsigned ROW, typename Function, typename Matrix>
constexpr void fillRows(Function generator, Matrix& matrix) {
  constexpr auto COL = matrix.size() - 1;
  fillCols<ROW, COL>(generator, matrix);

  if constexpr (ROW == 0) {
    return;
  } else {
    fillRows<ROW - 1>(generator, matrix);
  }
}

template <unsigned SIZE, typename Function>
constexpr matrix_t<SIZE> generateMatrix(Function generator) {
  matrix_t<SIZE> ret{};
  if constexpr (!SIZE) {
    return ret;
  } else {
    fillRows<SIZE - 1>(generator, ret);
    return ret;
  }
}
