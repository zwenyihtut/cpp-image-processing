#include "libpng_wrapper.hpp"
#include <stdexcept>

namespace libpng {
void writeHeaderChunk(png_structp pngPtr,
                      png_infop infoPtr,
                      const HeaderChunk& header) {
  png_set_IHDR(pngPtr, infoPtr, header.width, header.height, header.bitDepth,
               header.colorType, header.interlaceType, header.compressionType,
               header.filterMethod);
}

std::pair<png_structp, png_infop> initializeWriteStructAndInfoPtr() {
  png_structp pngPtr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!pngPtr) {
    throw std::runtime_error("Unable to create image");
  }

  png_infop infoPtr = png_create_info_struct(pngPtr);
  if (!infoPtr) {
    png_destroy_write_struct(&pngPtr, NULL);
    throw std::runtime_error("Unable to create image");
  }
  return { pngPtr, infoPtr };
}

bool verifyFormat(FILE* file) {
  unsigned char magicBytes[8];

  const size_t bytesRead = fread(magicBytes, 1, sizeof(magicBytes), file);
  if (bytesRead == 0)
    return false;
  const bool isPng = !png_sig_cmp(magicBytes, 0, sizeof(magicBytes));
  return isPng;
}

Image read(const std::string& name) {
  FILE* file(fopen(name.c_str(), "rb"));
  if (!libpng::verifyFormat(file)) {
    throw std::runtime_error(name + " is not a valid PNG file");
  }
  Image img;
  img.png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!img.png) {
    throw std::runtime_error("Failed to create png pointer");
  }

  img.info = png_create_info_struct(img.png);

  if (!img.info) {
    png_destroy_read_struct(&img.png, NULL, NULL);
    throw std::runtime_error("Failed to create png info pointer");
  }

  png_init_io(img.png, file);
  png_set_sig_bytes(img.png, 8);
  png_read_png(img.png, img.info, PNG_TRANSFORM_IDENTITY, NULL);
  img.buffer = png_get_rows(img.png, img.info);
  fclose(file);
  return img;
}

void clean(Image& img) {
  png_destroy_read_struct(&img.png, &img.info, NULL);
}
}  // namespace libpng
