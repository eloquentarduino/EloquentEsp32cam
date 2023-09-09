#ifndef ELOQUENT_ESP32CAM_CAMERA_RESOLUTION
#define ELOQUENT_ESP32CAM_CAMERA_RESOLUTION

#include <esp_camera.h>


namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Handle methods related to frame size
             */
            class Resolution {
            public:
                framesize_t framesize;

                /**
                 * Constructor
                 */
                Resolution() {
                    vga();
                }

                /**
                 * 96 x 96
                 */
                void _96x96() {
                    framesize = FRAMESIZE_96X96;
                    width = 96;
                    height = 96;
                }

                /**
                 * 96 x 96
                 */
                void square96() {
                    _96x96();
                }

                /**
                 * 96 x 96
                 */
                void yolo() {
                    _96x96();
                }

                /**
                 * 160 x 120
                 */
                void qqvga() {
                    framesize = FRAMESIZE_QQVGA;
                    width = 160;
                    height = 120;
                }

                /**
                 * 176 x 144
                 */
                void qcif() {
                    framesize = FRAMESIZE_QCIF;
                    width = 176;
                    height = 144;
                }

                /**
                 * 240 x 176
                 */
                void hqvga() {
                    framesize = FRAMESIZE_HQVGA;
                    width = 240;
                    height = 176;
                }

                /**
                 * 240 x 240
                 */
                void _240x240() {
                    framesize = FRAMESIZE_240X240;
                    width = 240;
                    height = 240;
                }

                /**
                 * 240 x 240
                 */
                void square240() {
                    _240x240();
                }

                /**
                 * 240 x 240
                 */
                void face() {
                    _240x240();
                }

                /**
                 * 320 x 240
                 */
                void qvga() {
                    framesize = FRAMESIZE_QVGA;
                    width = 320;
                    height = 240;
                }

                /**
                 * 400 x 296
                 */
                void cif() {
                    framesize = FRAMESIZE_CIF;
                    width = 400;
                    height = 296;
                }

                /**
                 * 480 x 320
                 */
                void hvga() {
                    framesize = FRAMESIZE_HVGA;
                    width = 480;
                    height = 320;
                }

                /**
                 * 640 x 480
                 */
                void vga() {
                    framesize = FRAMESIZE_VGA;
                    width = 640;
                    height = 480;
                }

                /**
                 * 800 x 600
                 */
                void svga() {
                    framesize = FRAMESIZE_SVGA;
                    width = 800;
                    height = 600;
                }

                /**
                 * 1024 x 768
                 */
                void xga() {
                    framesize = FRAMESIZE_XGA;
                    width = 1204;
                    height = 768;
                }

                /**
                 * 1280 x 720
                 */
                void hd() {
                    framesize = FRAMESIZE_HD;
                    width = 1280;
                    height = 720;
                }

                /**
                 * 1280 x 1024
                 */
                void sxga() {
                    framesize = FRAMESIZE_SXGA;
                    width = 1280;
                    height = 1024;
                }

                /**
                 * 1600 x 1200
                 */
                void uxga() {
                    framesize = FRAMESIZE_UXGA;
                    width = 1600;
                    height = 1200;
                }

                /**
                 * 1920 x 1080
                 */
                void fhd() {
                    framesize = FRAMESIZE_FHD;
                    width = 1920;
                    height = 1080;
                }

                /**
                 * 720 x 1280
                 */
                void p_hd() {
                    framesize = FRAMESIZE_P_HD;
                    width = 720;
                    height = 1280;
                }

                /**
                 * 864 x 1536
                 */
                void p_3mp() {
                    framesize = FRAMESIZE_P_3MP;
                    width = 864;
                    height = 1536;
                }

                /**
                 * 2048 x 1536
                 */
                void qxga() {
                    framesize = FRAMESIZE_QXGA;
                    width = 2048;
                    height = 1536;
                }

                /**
                 * 2560 x 1440
                 */
                void qhd() {
                    framesize = FRAMESIZE_QHD;
                    width = 2560;
                    height = 1440;
                }

                /**
                 * 2560 x 1600
                 */
                void wqxga() {
                    framesize = FRAMESIZE_WQXGA;
                    width = 2560;
                    height = 1600;
                }

                /**
                 * 1080 x 1920
                 */
                void p_fhd() {
                    framesize = FRAMESIZE_P_FHD;
                    width = 1080;
                    height = 1920;
                }

                /**
                 * 2560 x 1920
                 */
                void qsxga() {
                    framesize = FRAMESIZE_QSXGA;
                    width = 2560;
                    height = 1920;
                }

                /**
                 * Get width of captured image
                 * @return
                 */
                size_t getWidth() {
                    return width;
                }

                /**
                 * Get height of captured image
                 * @return
                 */
                size_t getHeight() {
                    return height;
                }

                /**
                 *
                 * @return
                 */
                bool isXGA() {
                    return framesize == FRAMESIZE_XGA;
                }

            protected:
                uint16_t width;
                uint16_t height;
            };
        }
    }
}

#endif
