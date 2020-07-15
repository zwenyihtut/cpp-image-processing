#include <algorithm>
#include <functional>
#include "catch.hpp"
#include "image_traverser.hpp"

int doNothing(size_t, size_t) {
  return 0;
}
class MockRow {
public:
  using pixel_t = int;
  using PixelGenerator = std::function<pixel_t(size_t, size_t)>;

  MockRow(PixelGenerator generator, size_t y) : mGenerator(generator), mY(y) {}
  MockRow() = default;
  pixel_t operator[](size_t x) const { return mGenerator(x, mY); }
private:
  size_t mY;
  PixelGenerator mGenerator = doNothing;
};

class MockImage {
 public:
  using pixel_t = int;
  using row_t   = MockRow;
  using PixelGenerator = std::function<pixel_t(size_t, size_t)>;

  MockImage(PixelGenerator generator) : mGenerator(generator) {}

  constexpr size_t width() const { return 100; }
  constexpr size_t height() const { return 100; }
  MockRow operator[](size_t y) const { return MockRow(mGenerator, y); }

 private:
  PixelGenerator mGenerator;
};


TEST_CASE("ImageTraverser", "[ImageTraverser]") {
  MockImage basicImage([](size_t, size_t) { return 1; });

  SECTION("works with ranged-for loops") {
    unsigned counter = 0;
    for (const auto& [p, i]  : pixels(basicImage)) {
      counter += p;
    }
    REQUIRE(counter == (basicImage.width() * basicImage.height()));
  }

  SECTION("works with stl algorithms") {
    auto traverser = pixels(basicImage);
    auto allGivesOne =
        std::all_of(traverser.begin(), traverser.end(), [](auto value) -> bool {
            const auto& [p, _] = value;
            return p == 1;
        });
    REQUIRE(allGivesOne);
  }
}
