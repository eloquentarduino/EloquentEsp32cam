//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_SETFRAMESIZE_H
#define ELOQUENTESP32CAM_SETFRAMESIZE_H

#include <esp_camera.h>


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Handle methods related to frame size
             */
            class SetsResolution {
            public:
                framesize_t framesize;

                /**
                 * Constructor
                 */
                SetsResolution() {
                    vga();
                }


                /**
                 * 96 x 96
                 */
                void _96x96() {
                    framesize = FRAMESIZE_96X96;
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
                }

                /**
                 * 176 x 144
                 */
                void qcif() {
                    framesize = FRAMESIZE_QCIF;
                }

                /**
                 * 240 x 176
                 */
                void hqvga() {
                    framesize = FRAMESIZE_HQVGA;
                }

                /**
                 * 240 x 240
                 */
                void _240x240() {
                    framesize = FRAMESIZE_240X240;
                }

                /**
                 * 320 x 240
                 */
                void qvga() {
                    framesize = FRAMESIZE_QVGA;
                }

                /**
                 * 400 x 296
                 */
                void cif() {
                    framesize = FRAMESIZE_CIF;
                }

                /**
                 * 480 x 320
                 */
                void hvga() {
                    framesize = FRAMESIZE_HVGA;
                }

                /**
                 * 640 x 480
                 */
                void vga() {
                    framesize = FRAMESIZE_VGA;
                }

                /**
                 * 800 x 600
                 */
                void svga() {
                    framesize = FRAMESIZE_SVGA;
                }

                /**
                 * 1024 x 768
                 */
                void xga() {
                    framesize = FRAMESIZE_XGA;
                }

                /**
                 * 1280 x 720
                 */
                void hd() {
                    framesize = FRAMESIZE_HD;
                }

                /**
                 * 1280 x 1024
                 */
                void sxga() {
                    framesize = FRAMESIZE_SXGA;
                }

                /**
                 * 1600 x 1200
                 */
                void uxga() {
                    framesize = FRAMESIZE_UXGA;
                }

                /**
                 * 1920 x 1080
                 */
                void fhd() {
                    framesize = FRAMESIZE_FHD;
                }

                /**
                 * 720 x 1280
                 */
                void p_hd() {
                    framesize = FRAMESIZE_P_HD;
                }

                /**
                 * 864 x 1536
                 */
                void p_3mp() {
                    framesize = FRAMESIZE_P_3MP;
                }

                /**
                 * 2048 x 1536
                 */
                void qxga() {
                    framesize = FRAMESIZE_QXGA;
                }

                /**
                 * 2560 x 1440
                 */
                void qhd() {
                    framesize = FRAMESIZE_QHD;
                }

                /**
                 * 2560 x 1600
                 */
                void wqxga() {
                    framesize = FRAMESIZE_WQXGA;
                }

                /**
                 * 1080 x 1920
                 */
                void p_fhd() {
                    framesize = FRAMESIZE_P_FHD;
                }

                /**
                 * 2560 x 1920
                 */
                void qsxga() {
                    framesize = FRAMESIZE_QSXGA;
                }

                /**
                 * Get width of captured image
                 * @return
                 */
                size_t getWidth() {
                    switch (framesize) {
                        case FRAMESIZE_96X96:
                            return 96;
                        case FRAMESIZE_QQVGA:
                            return 160;
                        case FRAMESIZE_QCIF:
                            return 176;
                        case FRAMESIZE_HQVGA:
                            return 240;
                        case FRAMESIZE_240X240:
                            return 240;
                        case FRAMESIZE_QVGA:
                            return 320;
                        case FRAMESIZE_CIF:
                            return 400;
                        case FRAMESIZE_HVGA:
                            return 480;
                        case FRAMESIZE_VGA:
                            return 640;
                        case FRAMESIZE_SVGA:
                            return 800;
                        case FRAMESIZE_XGA:
                            return 1024;
                        case FRAMESIZE_HD:
                            return 1280;
                        case FRAMESIZE_SXGA:
                            return 1280;
                        case FRAMESIZE_UXGA:
                            return 1600;
                        case FRAMESIZE_FHD:
                            return 1920;
                        case FRAMESIZE_P_HD:
                            return 720;
                        case FRAMESIZE_P_3MP:
                            return 864;
                        case FRAMESIZE_QXGA:
                            return 2048;
                        case FRAMESIZE_QHD:
                            return 2560;
                        case FRAMESIZE_WQXGA:
                            return 2560;
                        case FRAMESIZE_P_FHD:
                            return 1080;
                        case FRAMESIZE_QSXGA:
                            return 2560;
                        default:
                            return 0;
                    }
                }

                /**
                 * Get height of captured image
                 * @return
                 */
                size_t getHeight() {
                    switch (framesize) {
                        case FRAMESIZE_96X96:
                            return 96;
                        case FRAMESIZE_QQVGA:
                            return 120;
                        case FRAMESIZE_QCIF:
                            return 144;
                        case FRAMESIZE_HQVGA:
                            return 176;
                        case FRAMESIZE_240X240:
                            return 240;
                        case FRAMESIZE_QVGA:
                            return 240;
                        case FRAMESIZE_CIF:
                            return 296;
                        case FRAMESIZE_HVGA:
                            return 320;
                        case FRAMESIZE_VGA:
                            return 480;
                        case FRAMESIZE_SVGA:
                            return 600;
                        case FRAMESIZE_XGA:
                            return 768;
                        case FRAMESIZE_HD:
                            return 720;
                        case FRAMESIZE_SXGA:
                            return 1024;
                        case FRAMESIZE_UXGA:
                            return 1200;
                        case FRAMESIZE_FHD:
                            return 1080;
                        case FRAMESIZE_P_HD:
                            return 1280;
                        case FRAMESIZE_P_3MP:
                            return 1536;
                        case FRAMESIZE_QXGA:
                            return 1536;
                        case FRAMESIZE_QHD:
                            return 1440;
                        case FRAMESIZE_WQXGA:
                            return 1600;
                        case FRAMESIZE_P_FHD:
                            return 1920;
                        case FRAMESIZE_QSXGA:
                            return 1920;
                        default:
                            return 0;
                    }
                }

                /**
                 *
                 * @return
                 */
                bool isXGA() {
                    return framesize == FRAMESIZE_XGA;
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SETFRAMESIZE_H
