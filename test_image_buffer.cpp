#include "catch.hpp"
#include "image_file_buffer.hpp"

using VecBuffer = std::vector<std::vector<double>>;

constexpr size_t TEST_IMAGE_WIDTH = 10;
constexpr size_t TEST_IMAGE_HEIGHT = 20;
class TestBuffer : public ImageBuffer<TestBuffer, VecBuffer> {
 public:

  TestBuffer() : mBuffer(10, std::vector<double>(10, 5)) {}

  size_t width() const { return TEST_IMAGE_WIDTH; }
  size_t height() const { return TEST_IMAGE_HEIGHT; }

 private:
  VecBuffer mBuffer;
};

template <typename T, typename B>
void test_dimensions(const ImageBuffer<T, B>& buffer) {
  REQUIRE(buffer.width() == TEST_IMAGE_WIDTH);
  REQUIRE(buffer.height() == TEST_IMAGE_HEIGHT);
}

TEST_CASE("", "[ImageFile]") {
  TestBuffer testBuffer;
  test_dimensions(testBuffer);
}
