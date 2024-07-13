#ifndef ELOQUENT_ESP32CAM_CAMERA_PIXFORMAT_H
#define ELOQUENT_ESP32CAM_CAMERA_PIXFORMAT_H

#include <esp_camera.h>


namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * 
             */
            class Pixformat {
            public:
                pixformat_t format;

                /**
                 * 
                 */
                Pixformat() : format(PIXFORMAT_JPEG) {

                }

                /**
                 * Alias for grayscale
                 */
                void gray() {
                    grayscale();
                }

                /**
                 *
                 */
                void grayscale() {
                    format = PIXFORMAT_GRAYSCALE;
                }

                /**
                 * 
                 */
                void rgb565() {
                    format = PIXFORMAT_RGB565;
                }

                /**
                 * 
                 */
                void rgb888() {
                    format = PIXFORMAT_RGB888;
                }

                /**
                 * 
                 */
                void jpeg() {
                    format = PIXFORMAT_JPEG;
                }

                /**
                 * Test if grayscale
                 */
                inline bool isGray() {
                    return is(PIXFORMAT_GRAYSCALE);
                }

                /**
                 * Test if RGB565
                 */
                inline bool isRGB565() {
                    return is(PIXFORMAT_RGB565);
                }

                /**
                 * Test if RGB888
                 */
                inline bool isRGB888() {
                    return is(PIXFORMAT_RGB888);
                }

                /**
                 * Test if grayscale
                 */
                inline bool isJpeg() {
                    return is(PIXFORMAT_JPEG);
                }

                /**
                 * Test if given format
                 */
                inline bool is(pixformat_t fmt) {
                    return format == fmt;
                }

                /**
                 * Get bytes per pixel
                 * @return
                 */
                inline uint8_t bpp() {
                    return isGray() ? 1 : (isRGB888() ? 3 : 2);
                }
            };
        }
    }
}

#endif