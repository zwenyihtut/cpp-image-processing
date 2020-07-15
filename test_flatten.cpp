#include "catch.hpp"
#include "flatten.hpp"
#include <array>

TEST_CASE("flattened", "[flattened]") {
  int counter = 0;

  SECTION("works with C-arrays") {
    int arr[3][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

    for (auto num : flattened(arr)) {
      REQUIRE(num == counter);
      ++counter;
    }
  }

  SECTION("works with std::arrays") {
    std::array<std::array<int, 3>, 3> arr2 = {
        {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}}};

    counter = 0;
    for (auto num : flattened(arr2)) {
      REQUIRE(num == counter);
      ++counter;
    }
  }

  SECTION("works with uneven rows") {
    std::vector<std::vector<int>> unevenRows = {
        {{0, 1, 2}, {3, 4, 5, 6, 7}, {}, {8, 9, 10, 11}, {12, 13}, {}}};

    counter = 0;
    for (auto num : flattened(unevenRows)) {
      REQUIRE(num == counter);
      ++counter;
    }
  }

  SECTION("works with empty containers") {
    std::vector<std::vector<int>> empty;

    counter = 0;
    for (auto val : flattened(empty)) {
      ++counter;
    }
    REQUIRE(counter == 0);
  }
}
