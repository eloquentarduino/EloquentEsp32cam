//
// Created by Simone on 12/12/22.
//

#ifndef ELOQUENTESP32CAM_COLORBLOBDETECTOR_H
#define ELOQUENTESP32CAM_COLORBLOBDETECTOR_H


#include "../../traits/HasErrorMessage.h"
#include "../../traits/BenchmarksCode.h"
#include "../JpegDecoder.h"
#include "../ConnectedComponent.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Applications {
            class ColorBlobDetector : public HasErrorMessage, public BenchmarksCode {
            public:
                bool mask[MAX_RESOLUTION_HEIGHT / 8][MAX_RESOLUTION_WIDTH / 8];
                uint16_t maskCount;
                uint16_t width;
                uint16_t height;
                ConnectedComponent<ColorBlobDetector> blob;
                ColorBlobDetector *next;

                /**
                 *
                 * @param y
                 * @param cb
                 * @param cr
                 */
                ColorBlobDetector(uint8_t y, uint8_t cb, uint8_t cr, uint8_t tol = 10) :
                    blob(*this),
                    maskCount(0),
                    _y(y),
                    _cb(cb),
                    _cr(cr),
                    _tol(tol),
                    _minArea(100),
                    width(0),
                    height(0),
                    next(NULL) {

                }

                /**
                 * Set tollerance
                 *
                 * @param tol
                 */
                void tollerate(uint8_t tol) {
                    _tol = tol;
                }

                /**
                 * Skip blob localization (slow) if not enough
                 * pixels match color
                 * @param area
                 */
                void setMinArea(size_t area) {
                    _minArea = area / 64;
                }

                /**
                 *
                 * @return
                 */
                inline uint16_t getWidth() {
                    return width;
                }

                /**
                 *
                 * @return
                 */
                inline uint16_t getHeight() {
                    return height;
                }

                /**
                 *
                 * @param x
                 * @param y
                 * @return
                 */
                inline bool isForeground(uint16_t x, uint16_t y) {
                    return mask[y][x];
                }

                /**
                 * Chain detectors
                 * (mainly for internal use)
                 *
                 * @param next
                 */
                inline void chain(ColorBlobDetector& next) {
                    if (this->next)
                        this->next->chain(next);
                    else
                        this->next = &next;
                }

                /**
                 * Get Luma component
                 *
                 * @return
                 */
                inline uint8_t getLuma() {
                    return _y;
                }

                /**
                 * Get Luma component
                 *
                 * @return
                 */
                inline uint8_t getCb() {
                    return _cb;
                }

                /**
                 * Get Luma component
                 *
                 * @return
                 */
                inline uint8_t getCr() {
                    return _cr;
                }

                /**
                 * Get tollerance
                 *
                 * @return
                 */
                inline uint8_t getTol() {
                    return _tol;
                }

                /**
                 * Get min area of blob
                 *
                 * @return
                 */
                inline size_t getMinArea() {
                    return _minArea * 64;
                }


                /**
                 * Set option from key-value pair
                 *
                 * @param key
                 * @param value
                 */
                bool set(String key, float value) {
                    if (key == "y")
                        _y = value;
                    else if (key == "cb")
                        _cb = value;
                    else if (key == "cr")
                        _cr = value;
                    else if (key == "tol")
                        tollerate(value);
                    else if (key == "min-area")
                        setMinArea(value);
                    else
                        return setErrorMessage(String("Unknown option: ") + key);

                    return isOk();
                }

                /**
                 * Update target color
                 *
                 * @param y
                 * @param cb
                 * @param cr
                 */
                void updateColor(uint8_t y, uint8_t cb, uint8_t cr) {
                    _y = y;
                    _cb = cb;
                    _cr = cr;
                }

                /**
                 * Detect color blog in Jpeg frame
                 *
                 * @param decoder
                 * @return
                 */
                bool detect(JpegDecoder& decoder) {
                    startBenchmark();

                    maskCount = 0;
                    width = decoder.luma.width;
                    height = decoder.luma.height;

                    // reset mask
                    memset(mask, 0, width * height);

                    size_t i = 0;

                    for (uint16_t row = 0; row < height; row++) {
                        for (uint16_t col = 0; col < width; col++) {
                            i += 1;
                            const size_t j = i - 1;
                            const int16_t yi = decoder.luma.pixels[j];

                            if (abs(yi - _y) > _tol)
                                continue;

                            const int16_t cbi = decoder.cb.pixels[j];

                            if (abs(cbi - _cb) > _tol)
                                continue;

                            const int16_t cri = decoder.cr.pixels[j];

                            if (abs(cri - _cr) > _tol)
                                continue;

                            mask[row][col] = true;
                            maskCount += 1;
                        }
                    }

                    if (maskCount < _minArea) {
                        stopBenchmark();

                        return setErrorMessage("Not enough pixels");
                    }

                    if (!blob.localize()) {
                        startBenchmark();

                        return setErrorMessage(blob.getErrorMessage());
                    }

                    stopBenchmark();

                    return setErrorMessage(blob.area >= _minArea ? "" : String("No blob found of at least given area (") + blob.area + " found)");
                }

                /**
                 *
                 * @tparam Printer
                 * @param printer
                 */
                template<typename Printer>
                void printTo(Printer& printer) {
                    for (int i = 0; i < width + 2; i++)
                        printer.print('-');

                    printer.print('\n');

                    for (uint16_t y = 0; y < height; y++) {
                        printer.print('|');

                        for (uint16_t x = 0; x < width; x++)
                            printer.print(mask[y][x] ? 'x' : ' ');

                        printer.print('|');
                        printer.print('\n');
                    }

                    for (int i = 0; i < width + 2; i++)
                        printer.print('-');

                    printer.print('\n');
                }

                /**
                 * Get detection color as RGB
                 *
                 * @return
                 */
                String toRGB() {
                    int16_t red   = 1.164 * _y + 1.596 * _cr;
                    int16_t green = 1.164 * _y - 0.813 * _cr - 0.392 * _cb;
                    int16_t blue  = 1.164 * _y + 2.017 * _cb;

                    if (red < 0) red = 0;
                    if (green < 0) green = 0;
                    if (blue < 0) blue = 0;
                    if (red > 255) red = 255;
                    if (green > 255) green = 255;
                    if (blue > 255) blue = 255;

                    char hex[7];
                    sprintf(hex, "%02X%02X%02X\0", red, green, blue);

                    return String(hex);
                }

                /**
                 * Convert mask to string
                 *
                 * @return
                 */
                String toString() {
                    static char buffer[MAX_RESOLUTION_WIDTH / 8 * MAX_RESOLUTION_HEIGHT / 8 + 1] = {'0'};
                    uint16_t k = 0;

                    for (uint16_t i = 0; i < height; i++) {
                        for (uint16_t j = 0; j < width; j++)
                            buffer[k++] = mask[i][j] ? '1' : '0';

                    }

                    buffer[width * height + 1] = '\0';

                    return String(buffer);
                }

                /**
                 *
                 * @return
                 */
                String bboxAsJson() {
                    return isOk() ? blob.toJson() : "{}";
                }


            protected:
                uint8_t _y;
                uint8_t _cb;
                uint8_t _cr;
                uint8_t _tol;
                size_t _minArea;
            };
        }
    }
}


#endif //ELOQUENTESP32CAM_COLORBLOBDETECTOR_H
