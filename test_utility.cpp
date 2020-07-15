#include "catch.hpp"
#include "utility.hpp"

TEST_CASE("findDirection works", "[utility]") {
  REQUIRE(findDirection(0) == Direction::RIGHT);
  REQUIRE(findDirection(0.01) == Direction::RIGHT);

  REQUIRE(findDirection(M_PI) == Direction::LEFT);
  REQUIRE(findDirection(-M_PI) == Direction::LEFT);

  REQUIRE(findDirection(-M_PI_2) == Direction::BOTTOM);
  REQUIRE(findDirection(-M_PI_2 + 0.01) == Direction::BOTTOM);

  REQUIRE(findDirection(M_PI_2) == Direction::TOP);
  REQUIRE(findDirection(M_PI_2 - 0.01) == Direction::TOP);

  REQUIRE(findDirection(M_PI_4) == Direction::TOP_RIGHT);
  REQUIRE(findDirection(-M_PI_4) == Direction::BOTTOM_RIGHT);

  REQUIRE(findDirection(3 * M_PI_4) == Direction::TOP_LEFT);
  REQUIRE(findDirection(-3 * M_PI_4) == Direction::BOTTOM_LEFT);
}
