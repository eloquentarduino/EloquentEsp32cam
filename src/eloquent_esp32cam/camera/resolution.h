#ifndef ELOQUENT_ESP32CAM_CAMERA_RESOLUTION
#define ELOQUENT_ESP32CAM_CAMERA_RESOLUTION

#include <esp_camera.h>
#include "../extra/time/benchmark.h"

using Eloquent::Extra::Time::Benchmark;


namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Handle methods related to frame size
             */
            class Resolution {
            public:
                framesize_t framesize;
                Benchmark benchmark;

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
                void phd() {
                    framesize = FRAMESIZE_P_HD;
                    width = 720;
                    height = 1280;
                }

                /**
                 * 864 x 1536
                 */
                void p3mp() {
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
                void pfhd() {
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
                 * @brief Set resolution
                 * @param resolution
                 */
                void set(framesize_t resolution) {
                    switch (resolution) {
                        case FRAMESIZE_96X96: _96x96(); break;
                        case FRAMESIZE_QQVGA: square96(); break;
                        case FRAMESIZE_QCIF: qcif(); break;
                        case FRAMESIZE_HQVGA: hqvga(); break;
                        case FRAMESIZE_240X240: _240x240(); break;
                        case FRAMESIZE_QVGA: qvga(); break;
                        case FRAMESIZE_CIF: cif(); break;
                        case FRAMESIZE_HVGA: hvga(); break;
                        case FRAMESIZE_VGA: vga(); break;
                        case FRAMESIZE_SVGA: svga(); break;
                        case FRAMESIZE_XGA: xga(); break;
                        case FRAMESIZE_HD: hd(); break;
                        case FRAMESIZE_SXGA: sxga(); break;
                        case FRAMESIZE_UXGA: uxga(); break;
                        case FRAMESIZE_FHD: fhd(); break;
                        case FRAMESIZE_P_HD: phd(); break;
                        case FRAMESIZE_P_3MP: p3mp(); break;
                        case FRAMESIZE_QXGA: qxga(); break;
                        case FRAMESIZE_QHD: qhd(); break;
                        case FRAMESIZE_WQXGA: wqxga(); break;
                        case FRAMESIZE_P_FHD: pfhd(); break;
                        case FRAMESIZE_QSXGA: qsxga(); break;
                        default:
                            ESP_LOGE("Resolution", "Unsupported resolution: %d", (int) resolution);
                            return;
                    }
                    
                    sensor_t *sensor = esp_camera_sensor_get();
                    sensor->set_framesize(sensor, resolution);
                }

                /**
                 *
                 * @return
                 */
                bool isXGA() {
                    return framesize == FRAMESIZE_XGA;
                }
                
                /**
                 * @brief Run function at given resolution, that revert back
                 * @todo test
                 * @param resolution
                 * @param callback
                 */
                template<typename Callback>
                void at(framesize_t resolution, Callback callback) {
                    framesize_t old = framesize;
                    
                    // how long does it take to switch resolution?
                    benchmark.benchmark([this, resolution]() {
                        set(resolution);
                    });
                    callback();
                    set(old);
                }

                /**
                 * Print help message
                 */
                void help() {
                    /*LOG_HELP("AVAILABLE RESOLUTIONS");
                    LOG_HELP(" > yolo (96x96)");
                    LOG_HELP(" > qqvga (160x120)");
                    LOG_HELP(" > qcif (176x144)");
                    LOG_HELP(" > hqvga (240x176)");
                    LOG_HELP(" > face (240x240)");
                    LOG_HELP(" > qvga (320x240)");
                    LOG_HELP(" > cif (400x296)");
                    LOG_HELP(" > hvga (480x320)");
                    LOG_HELP(" > vga (640x480)");
                    LOG_HELP(" > svga (800x600)");
                    LOG_HELP(" > xga (1204x768)");
                    LOG_HELP(" > hd (1280x720)");
                    LOG_HELP(" > sxga (1280x1024)");
                    LOG_HELP(" > uxga (1600x1200)");
                    LOG_HELP(" > fhd (1920x1080)");
                    LOG_HELP(" > phd (720x1280)");
                    LOG_HELP(" > p3mp (864x1536)");
                    LOG_HELP(" > qxga (2048x1536)");
                    LOG_HELP(" > qhd (2560x1440)");
                    LOG_HELP(" > wqxga (2560x1600)");
                    LOG_HELP(" > pfhd (1080x1920)");
                    LOG_HELP(" > qsxga (2560x1920)");*/
                }

            protected:
                uint16_t width;
                uint16_t height;
            };
        }
    }
}

#endif
