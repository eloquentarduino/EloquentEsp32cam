#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_BBOX_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_BBOX_H

namespace eloq {
    namespace ei {
        class bbox_t {
            public:
                String label;
                float proba;
                uint16_t x;
                uint16_t y;
                uint16_t x1;
                uint16_t y1;
                uint16_t x2;
                uint16_t y2;
                uint16_t cx;
                uint16_t cy;
                uint16_t width;
                uint16_t height;

                /**
                 * Constructor
                 */
                bbox_t(String label_, float proba_, uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_) :
                    label(label_),
                    proba(proba_) {
                        setDimensions(x_, y_, width_, height_);
                    }

                /**
                 * 
                 */
                void setDimensions(uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_) {
                    x = x1 = x_;
                    y = y1 = y_;
                    width = width_;
                    height = height_;
                    x2 = x1 + width;
                    y2 = y1 + height;
                    cx = (x1 + x2) / 2;
                    cy = (y1 + y2) / 2;
                }
        };
    }
}

#endif