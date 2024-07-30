#if defined(__JPEGDEC__)
#ifndef ELOQUENTESP32CAM_JPEGDECWRAPPER_H
#define ELOQUENTESP32CAM_JPEGDECWRAPPER_H

#include <JPEGDEC.h>
#include "../mem.h"
#include "../camera/camera.h"
#include "../extra/exception.h"
#include "../extra/time/benchmark.h"

using eloq::camera;
using Eloquent::Error::Exception;
using Eloquent::Extra::Time::Benchmark;

/**
 * Sadly we need to declare a global function
 * to handle decoding
 */
int __handleMCU__(jpeg_draw_tag *mcu);

namespace Eloquent {
    namespace Esp32cam {
        namespace JPEG {
            class JPEGDECWrapper {
            public:
                uint8_t *pixels;
                uint16_t outputWidth;
                uint16_t outputHeight;
                JPEGDEC jpeg;
                Exception exception;
                Benchmark benchmark;
                struct {
                    int16_t x;
                    int16_t y;
                    uint16_t width;
                    uint16_t height;
                } crop;

                /**
                 * Constructor
                 */
                JPEGDECWrapper() :
                    pixels(NULL),
                    exception("JPEGDEC"),
                    _allocatedSize(0),
                    _options(0) {
                    crop.x = 0;
                    crop.y = 0;
                    crop.width = 0;
                    crop.height = 0;
                }

                /**
                 * Todo: not working
                 */
                void onlyLuma() {
                    _options |= JPEG_LUMA_ONLY;
                }

                /**
                 * Decode at 1/2 resolution
                 */
                inline void half() {
                    _options |= JPEG_SCALE_HALF;
                }

                /**
                 * Decode at 1/4 resolution
                 */
                inline void quarter() {
                    _options |= JPEG_SCALE_QUARTER;
                }

                /**
                 * Decode at 1/8 resolution
                 */
                inline void eighth() {
                    _options |= JPEG_SCALE_EIGHTH;
                }

                /**
                 * Set divisor
                 */
                void divide(uint8_t divisor) {
                    switch (divisor) {
                        case 8:
                            eighth();
                            return;
                        case 4:
                            quarter();
                            return;
                        case 2:
                            half();
                            return;
                    }
                }

                /**
                 *
                 */
                Exception& decode() {
                    if (!camera.hasFrame())
                        return exception.set("No frame available");

                    if (!jpeg.openRAM(camera.frame->buf, camera.frame->len, __handleMCU__))
                        return exception.set(decodeErrorCode(jpeg.getLastError()));

                    const uint16_t inputWidth = camera.frame->width / divisor();
                    const uint16_t inputHeight = camera.frame->height / divisor();

                    outputWidth = crop.width ? crop.width : inputWidth;
                    outputHeight = crop.height ? crop.height : inputHeight;
                    alloc(outputWidth * outputHeight * bpp());
                    jpeg.setUserPointer((void*) this);

                    // allow for negative crop x/y
                    while (crop.x < 0)
                        crop.x += inputWidth;

                    while (crop.y < 0)
                        crop.y += inputHeight;

                    if (pixels == NULL)
                        return exception.set("Can't allocate decode buffer");

                    benchmark.benchmark([this]() {
                        exception.set(
                            !jpeg.decode(0, 0, _options) ?
                            decodeErrorCode(jpeg.getLastError()) : ""
                        );
                        jpeg.close();
                    });

                    return exception;
                }

                /**
                 * Get dimension divisor
                 */
                uint8_t divisor() {
                    return _options & JPEG_SCALE_EIGHTH ? 8 :
                        (_options & JPEG_SCALE_QUARTER ? 4 :
                            (_options & JPEG_SCALE_HALF ? 2 : 1));
                }

                /**
                 * Get number of decoded pixels
                 */
                inline size_t count() {
                    return outputWidth * outputHeight;
                }

                /**
                 * Get number of bytes
                 */
                inline size_t size() {
                    return count() * bpp();
                }

                /**
                 * Get decoded image width
                 */
                inline uint16_t width() {
                    return outputWidth;
                }

                /**
                 * Get decoded image height
                 */
                inline uint16_t height() {
                    return outputHeight;
                }

                /**
                 * Get bytes per pixel (grayscale = 1, rgb = 2)
                 */
                inline uint8_t bpp() {
                    return _options & JPEG_LUMA_ONLY ? 1 : 2;
                }

                /**
                 * Get pixels as uint16_t* buffer
                 */
                uint16_t* u16() {
                    return (uint16_t*) pixels;
                }

                /**
                 * Get pixels as uint8_t* buffer
                 */
                uint8_t* u8() {
                    return pixels;
                }

                /**
                 * Swap byte order
                 */
                void swapBytes() {
                    const size_t numBytes = size();

                    for (size_t i = 0; i < numBytes; i += 2) {
                        const uint8_t a = pixels[i];
                        const uint8_t b = pixels[i + 1];
                        pixels[i] = b;
                        pixels[i + 1] = a;
                    }
                }

                /**
                 * For internal use only.
                 */
                bool addMCU(uint16_t x, uint16_t y, uint8_t bpp, uint8_t *mcuPixels, uint16_t width) {
                    // easy case: no crop
                    if (crop.x == 0 && crop.y == 0 && crop.width == 0 && crop.height == 0) {
                        // todo: ephimeral decoding

                        const size_t outputOffset = y * outputWidth + x;

                        memcpy(pixels + outputOffset * bpp, mcuPixels, width * bpp);
                        return true;
                    }

                    // only get pixels inside crop area
                    // early stopping
                    if (crop.height > 0 && y >= crop.y + crop.height)
                        return false;

                    if (crop.y > 0 && y < crop.y)
                        return true;

                    if (x + width < crop.x)
                        return true;

                    if (crop.x > x) {
                        const uint16_t dx = crop.x - x;
                        mcuPixels += dx * bpp;
                        width -= dx;
                        x = crop.x;
                    }

                    if (crop.width > 0 && crop.width < width)
                        width = crop.width;

                    // todo: ephimeral decoding

                    const size_t outputOffset = (y - crop.y) * outputWidth + (x - crop.x);

                    memcpy(pixels + outputOffset * bpp, mcuPixels, width * bpp);
                    return true;
                }

            protected:
                size_t _allocatedSize;
                int _options;

                /**
                 * Allocate memory for decoded pixels
                 */
                void alloc(size_t size) {
                    if (pixels == NULL || _allocatedSize == 0) {
                        ESP_LOGD("JPEGDEG", "Allocating %d bytes for decoded pixels", size);
                        pixels = eloq::alloc<uint8_t>(size);
                        _allocatedSize = size;
                    }
                    else if (pixels != NULL && size != _allocatedSize) {
                        ESP_LOGD("JPEGDEG", "(Re)Allocating %d bytes for decoded pixels", size);
                        pixels = eloq::realloc<uint8_t>(pixels, size);
                        _allocatedSize = size;
                    }
                }

                /**
                 *
                 */
                String decodeErrorCode(int code) {
                    switch (code) {
                        case JPEG_INVALID_PARAMETER:
                            return "Invalid parameter";
                        case JPEG_DECODE_ERROR:
                            return "Decode error";
                        case JPEG_UNSUPPORTED_FEATURE:
                            return "Unsupported feature";
                        case JPEG_INVALID_FILE:
                            return "Invalid file";
                        default:
                            return "Unknown error";
                    }
                }
            };
        }
    }
}

namespace eloq {
    static Eloquent::Esp32cam::JPEG::JPEGDECWrapper jpegdec;
}

#endif //ELOQUENTESP32CAM_JPEGDECWRAPPER_H
#endif //__JPEGDEC__
