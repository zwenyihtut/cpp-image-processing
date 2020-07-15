#include "image_file_buffer.hpp"
#include <cassert>

std::string ImageFileBuffer::toColorTypeString(uint8_t colorType) {
  switch (colorType) {
    case PNG_COLOR_TYPE_RGB:
      return "RGB";
    case PNG_COLOR_TYPE_RGB_ALPHA:
      return "RGBA";
    case PNG_COLOR_TYPE_GRAY:
      return "GRAY";
    case PNG_COLOR_TYPE_PALETTE:
      return "PALETTE";
    default:
      return "unknown";
  }
}

ImageFileBuffer::ImageFileBuffer(const std::string& name) {
  this->load(name);
}

ImageFileBuffer::ImageFileBuffer(size_t width,
                                 size_t height,
                                 uint8_t colorType) {
  auto [pngPtr, infoPtr] = libpng::initializeWriteStructAndInfoPtr();

  mPngPtr = pngPtr;
  mInfoPtr = infoPtr;

  libpng::HeaderChunk header = { width, height, colorType };
  libpng::writeHeaderChunk(mPngPtr, mInfoPtr, header);

  // Allocate buffer
  auto rows = (png_bytepp)png_malloc(mPngPtr, height * (sizeof(png_bytep)));
  auto rowbytes = png_get_rowbytes(mPngPtr, mInfoPtr);
  for (png_uint_32 i = 0; i < height; i++) {
    rows[i] = (png_bytep)png_malloc(mPngPtr, rowbytes);
  }

  png_set_rows(mPngPtr, mInfoPtr, rows);
  mBuffer = png_get_rows(mPngPtr, mInfoPtr);
  
  mWriteReady = true;

  // need this so that we know to free on desctruction
  mBufferIsSelfManaged = true;
}

void ImageFileBuffer::load(const std::string& name) {
  FILE* file(fopen(name.c_str(), "rb"));

  if (!libpng::verifyFormat(file)) {
    throw std::runtime_error(name + " is not a valid PNG file");
  }

  mPngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!mPngPtr) {
    throw std::runtime_error("Failed to create png pointer");
  }

  mInfoPtr = png_create_info_struct(mPngPtr);

  if (!mInfoPtr) {
    png_destroy_read_struct(&mPngPtr, NULL, NULL);
    throw std::runtime_error("Failed to create png info pointer");
  }

  png_init_io(mPngPtr, file);
  png_set_sig_bytes(mPngPtr, 8);
  png_read_png(mPngPtr, mInfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
  mBuffer = png_get_rows(mPngPtr, mInfoPtr);
  fclose(file);
}

void ImageFileBuffer::save(const std::string& filename) {
  if (!mWriteReady) {
    // Destroy the already existing data
    png_destroy_read_struct(&mPngPtr, NULL, NULL);
    mPngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!mPngPtr) {
      throw std::runtime_error("Unable to write image");
    }
  }
  mWriteReady = true;

  FILE* file = fopen(filename.c_str(), "wb");
  png_init_io(mPngPtr, file);
  png_write_png(mPngPtr, mInfoPtr, PNG_TRANSFORM_IDENTITY, NULL);
  fclose(file);
}

unsigned ImageFileBuffer::width() const {
  return png_get_image_width(mPngPtr, mInfoPtr);
}
unsigned ImageFileBuffer::height() const {
  return png_get_image_height(mPngPtr, mInfoPtr);
}

std::string ImageFileBuffer::colorType() const {
  return ImageFileBuffer::toColorTypeString(
      png_get_color_type(mPngPtr, mInfoPtr));
}

uint8_t ImageFileBuffer::bitDepth() const {
  return png_get_bit_depth(mPngPtr, mInfoPtr);
}

ImageFileBuffer::~ImageFileBuffer() {
  if (mBufferIsSelfManaged) {
    const auto height = this->height();

    for (unsigned i = 0; i < height; ++i) {
      png_free(mPngPtr, mBuffer[i]);
    }

    png_free(mPngPtr, mBuffer);
  }
  if (mWriteReady)
    png_destroy_write_struct(&mPngPtr, &mInfoPtr);
  else
    png_destroy_read_struct(&mPngPtr, &mInfoPtr, NULL);
  assert(mPngPtr == NULL);
  assert(mInfoPtr == NULL);
}
