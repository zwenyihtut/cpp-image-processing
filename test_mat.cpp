#include "catch.hpp"
#include "mat.hpp"
#include "img.hpp"

TEST_CASE("Element get/set works", "[Mat]") {
  Mat<int> m({ 7 }, { 1, 2, 3, 4, 5, 6, 7 });
  REQUIRE(m[0] == 1);
  REQUIRE(m[1] == 2);

  Mat<int> m2({ 2, 2 }, { 1, 2, 3, 4 });
  REQUIRE(m2[0][0] == 1);
  REQUIRE(m2[0][1] == 2);
  REQUIRE(m2[1][0] == 3);
  REQUIRE(m2[1][1] == 4);

  Mat<int> m3({ 2, 1, 3 }, { 1, 2, 3, 4, 5, 6 });
  REQUIRE(m3[0][0][0] == 1);
  REQUIRE(m3[0][0][1] == 2);
  REQUIRE(m3[0][0][2] == 3);
  REQUIRE(m3[1][0][0] == 4);
  REQUIRE(m3[1][0][1] == 5);
  REQUIRE(m3[1][0][2] == 6);

  REQUIRE_THROWS(m3[1][1][1] = 0);
  REQUIRE_NOTHROW(m3[1][0][1] = 0);
  REQUIRE(m3[1][0][1] == 0);

  Mat<int> m4({ 1, 1, 1, 1 }, { 4 });
  REQUIRE(m4[0][0][0][0] == 4);
}

TEST_CASE("Size and dimension works", "[Mat]") {
  Mat<int> m({ 2, 1, 3 }, { 1, 2, 3, 4, 5, 6 });
  REQUIRE(m.size() == 2 * 1 * 3);
  REQUIRE(m.dimension(0) == 2);
  REQUIRE(m.dimension(1) == 1);
  REQUIRE(m.dimension(2) == 3);
}

TEST_CASE("Addition works", "[Mat]") {
  Mat<int> a({ 2, 2 }, { 1, 2, 3, 4 });
  Mat<int> b({ 2, 2 }, { 4, 3, 2, 1 });

  auto c = a + b;
  REQUIRE(c[0][0] == 5);
  REQUIRE(c[0][1] == 5);
  REQUIRE(c[1][0] == 5);
  REQUIRE(c[1][1] == 5);
}

TEST_CASE("Subtraction works", "[Mat]") {
  Mat<int> a({ 2, 2 }, { 1, 2, 3, 4 });
  Mat<int> b({ 2, 2 }, { 4, 3, 2, 1 });

  auto c = a - b;
  REQUIRE(c[0][0] == -3);
  REQUIRE(c[0][1] == -1);
  REQUIRE(c[1][0] == 1);
  REQUIRE(c[1][1] == 3);
}

TEST_CASE("read works", "[Mat]") {
  auto m = read("./images/yellow.png");
  REQUIRE(m.dimensions() == 3);
}
