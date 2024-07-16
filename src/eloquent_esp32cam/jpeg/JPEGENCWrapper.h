#ifndef __JPEGENC__
#error "You must include JPEGENC.h *before* eloquent_esp32cam"
#else

#ifndef ELOQUENTESP32CAM_JPEGENC
#define ELOQUENTESP32CAM_JPEGENC

#include "../mem.h"
#include "../camera/camera.h"
#include "../extra/exception.h"
#include "../extra/time/benchmark.h"

using eloq::camera;
using Eloquent::Error::Exception;
using Eloquent::Extra::Time::Benchmark;

namespace Eloquent {
    namespace Esp32cam {
        namespace JPEG {
            /**
             * Eloquent interface to JPEGENC library
             */
            class JPEGENCWrapper {
            public:
                uint8_t *bytes;
                JPEGENC jpeg;
                Exception exception;
                Benchmark benchmark;

                JPEGENCWrapper() :
                    exception("JPEGENC"),
                    _length(0),
                    _numBytes(0),
                    _pixelType(JPEGE_PIXEL_RGB565),
                    _quality(JPEGE_Q_HIGH),
                    _subsample(JPEGE_SUBSAMPLE_444) {

                }

                /**
                 *
                 */
                void fromRGB565() {
                    _pixelType = JPEGE_PIXEL_RGB565;
                }

                /**
                 *
                 */
                void fromGrayscale() {
                    _pixelType = JPEGE_PIXEL_GRAYSCALE;
                }

                /**
                 *
                 */
                void toBestQuality() {
                    _quality = JPEGE_Q_BEST;
                }

                /**
                 *
                 */
                void toGoodQuality() {
                    _quality = JPEGE_Q_HIGH;
                }

                /**
                 *
                 */
                void toMediumQuality() {
                    _quality = JPEGE_Q_MED;
                }

                /**
                 *
                 */
                void toLowQuality() {
                    _quality = JPEGE_Q_LOW;
                }

                /**
                 *
                 */
                void subsample() {
                    _subsample = JPEGE_SUBSAMPLE_420;
                }

                /**
                 * Allocate buffer for decoding
                 */
                Exception& allocate(size_t numBytes) {
                    bytes = eloq::realloc<uint8_t>(bytes, numBytes);
                    _numBytes = numBytes;

                    return bytes == NULL ? exception.set("Can't allocate memory") : exception.clear();
                }

                /**
                 * Encode raw pixels
                 */
                Exception& encode(uint8_t *pixels, uint16_t width, uint16_t height) {
                    benchmark.benchmark([this, pixels, width, height]() {
                        JPEGENCODE enc;

                        if (JPEGE_SUCCESS != jpeg.open(bytes, _numBytes)) {
                            exception.set("Cannot open JPEG");
                            return;
                        }

                        if (JPEGE_SUCCESS != jpeg.encodeBegin(&enc, width, height, _pixelType, _subsample, _quality)) {
                            exception.set("Cannot setup encoding");
                            return;
                        }

                        if (JPEGE_SUCCESS != jpeg.addFrame(&enc, pixels, width * bpp())) {
                            exception.set("Cannot add frame");
                            return;
                        }

                        _length = jpeg.close();
                    });

                    return _length > 0 ? exception.clear() : exception.set("Empty output");
                }

                /**
                 * Encode current frame
                 */
                Exception& encode() {
                    if (!camera.hasFrame())
                        return exception.set("Can't encode empty frame");

                    return this->encode(camera.frame->buf, camera.frame->width, camera.frame->height);
                }

                /**
                 * Get JPEG size
                 */
                inline size_t size() {
                    return _length;
                }

                /**
                 * Bytes per pixel
                 */
                inline uint8_t bpp() {
                    return _pixelType == JPEGE_PIXEL_GRAYSCALE ? 1 : 2;
                }

            protected:
                size_t _length;
                size_t _numBytes;
                uint8_t _pixelType;
                uint8_t _quality;
                uint8_t _subsample;
            };
        }
    }
}

namespace eloq {
    static Eloquent::Esp32cam::JPEG::JPEGENCWrapper jpegenc;
}
#endif

#endif