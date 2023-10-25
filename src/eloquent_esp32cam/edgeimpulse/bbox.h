#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_BBOX_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_BBOX_H

namespace eloq {
    class bbox_t {
        public:
            uint8_t value;
            uint16_t x;
            uint16_t y;
            uint16_t w;
            uint16_t h;
            uint16_t width;
            uint16_t height;

            /**
             * Constructor
             */
            bbox_t(uint8_t _val, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height) :
                value(_val),
                x(_x),
                y(_y),
                w(_width),
                h(_height),
                width(_width),
                height(_height) {
                    
                }
    };
}

#endif