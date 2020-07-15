#include "catch.hpp"
#include "image.hpp"
#include "pixels.hpp"

TEST_CASE("Image index operator functions correctly", "[Image]") {
  ImageFileBuffer file("./images/yellow.png");

  REQUIRE(file.colorType() == "RGB");
  REQUIRE(file.bitDepth() == 8);

  Image<PixelRGB8> img(file);

  REQUIRE(img.width() == 240);
  REQUIRE(img.height() == 240);

  for (size_t x = 0; x < 10; ++x) {
    for (size_t y = 0; y < 10; ++y) {
      REQUIRE(img[y][x].val()[0] == 255);
      REQUIRE(img[y][x].val()[1] == 255);
      REQUIRE(img[y][x].val()[2] == 0);
    }
  }
}
