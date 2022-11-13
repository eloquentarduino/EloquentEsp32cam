//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_SETFRAMESIZE_H
#define ELOQUENTESP32CAM_SETFRAMESIZE_H

#include <esp_camera.h>

namespace Eloquent {
    namespace Esp32cam {
        /**
         * Handle methods related to frame size
         */
        class SetFrameSize {
        public:

            void _96x96() {
                _framesize = FRAMESIZE_96X96;
            }

            void qqvga() {
                _framesize = FRAMESIZE_QQVGA;
            }

            void qcif() {
                _framesize = FRAMESIZE_QCIF;
            }

            void hqvga() {
                _framesize = FRAMESIZE_HQVGA;
            }

            void _240x240() {
                _framesize = FRAMESIZE_240X240;
            }

            void qvga() {
                _framesize = FRAMESIZE_QVGA;
            }

            void cif() {
                _framesize = FRAMESIZE_CIF;
            }

            void hvga() {
                _framesize = FRAMESIZE_HVGA;
            }

            void vga() {
                _framesize = FRAMESIZE_VGA;
            }

            void svga() {
                _framesize = FRAMESIZE_SVGA;
            }

            void xga() {
                _framesize = FRAMESIZE_XGA;
            }

            void hd() {
                _framesize = FRAMESIZE_HD;
            }

            void sxga() {
                _framesize = FRAMESIZE_SXGA;
            }

            void uxga() {
                _framesize = FRAMESIZE_UXGA;
            }

            void fhd() {
                _framesize = FRAMESIZE_FHD;
            }

            void p_hd() {
                _framesize = FRAMESIZE_P_HD;
            }

            void p_3mp() {
                _framesize = FRAMESIZE_P_3MP;
            }

            void qxga() {
                _framesize = FRAMESIZE_QXGA;
            }

            void qhd() {
                _framesize = FRAMESIZE_QHD;
            }

            void wqxga() {
                _framesize = FRAMESIZE_WQXGA;
            }

            void p_fhd() {
                _framesize = FRAMESIZE_P_FHD;
            }

            void qsxga() {
                _framesize = FRAMESIZE_QSXGA;
            }


            /**
             * Get width of captured image
             * @return
             */
            uint16_t getWidth() {
                switch (_framesize) {

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
            uint16_t getHeight() {
                switch (_framesize) {

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

        protected:
            framesize_t _framesize;
        };
    }
}

#endif //ELOQUENTESP32CAM_SETFRAMESIZE_H
