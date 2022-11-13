//
// Created by Simone on 07/11/22.
//

#ifndef ELOQUENTESP32CAM_DECODESJPEG_H
#define ELOQUENTESP32CAM_DECODESJPEG_H

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>
#include "../libs/picojpeg.h"
#include "./../defines.h"
#include "./HasErrorMessage.h"


/**
 *
 * @param dest
 * @param chunkSize
 * @param read
 * @param data
 * @return
 */
unsigned char pjpeg_need_bytes_callback(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data);



namespace Eloquent {
    namespace Esp32cam {

        /**
         * Extract pixel data from jpeg
         */
        class DecodesJpeg : public HasErrorMessage {
        public:
            uint8_t *jpegBuf;
            uint16_t jpegLength;

            struct {
                uint8_t buf[MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64] = {0};
                uint16_t len = 0;
            } Luma, Cb, Cr;

            /**
             *
             * @param len
             */
            void setJpeg(uint8_t *buf, uint16_t len) {
                jpegBuf = buf;
                jpegLength = len;
                _jpegOffset = 0;
            }

            /**
             *
             * @return
             */
            uint16_t getJpegOffset() {
                return _jpegOffset;
            }

            /**
             *
             * @param chunkSize
             */
            void consumeJpeg(uint16_t chunkSize) {
                _jpegOffset += chunkSize;
            }

            /**
             * Get image width
             *
             * @return
             */
            inline uint16_t getJpegWidth() {
                return _jpeg.m_width;
            }

            /**
             * Get image height
             *
             * @return
             */
            inline uint16_t getJpegHeight() {
                return _jpeg.m_height;
            }


            /**
             * Decode jpeg
             * @return
             */
            bool decode();

        protected:
            pjpeg_image_info_t _jpeg;
            uint16_t _jpegOffset;
        };
    }
}

#endif //ELOQUENTESP32CAM_DECODESJPEG_H
