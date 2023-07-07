//
// Created by Simone on 19/11/22.
//

#ifndef ELOQUENTESP32CAM_ROI_RECT_H
#define ELOQUENTESP32CAM_ROI_RECT_H

#include "./RoI.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            namespace RoI {
                class Rect : public RegionOfInterest {
                public:

                    /**
                     *
                     * @param x
                     * @param y
                     * @param width
                     * @param height
                     * @param threshold
                     */
                    Rect(float x, float y, float width, float height, float threshold = 0) {
                        setOrigin(x, y);
                        setSize(width, height);
                        setThreshold(threshold);
                    }

                    /**
                     * Typed constructor
                     * @tparam R
                     * @param rect
                     */
                    template<typename R>
                    Rect(R& rect) {
                        setOrigin(rect.x, rect.y);
                        setSize(rect.width, rect.height);
                        setThreshold(rect.threshold);
                    }

                    /**
                     * Set origin coordinates
                     * @param x
                     * @param y
                     */
                    inline void setOrigin(float x, float y) {
                        _x = x / 8;
                        _y = y / 8;
                    }

                    /**
                     * Set dimensions
                     * @param width
                     * @param height
                     */
                    inline void setSize(float width, float height) {
                        _w = width / 8;
                        _h = height / 8;
                    }

                    /**
                     * Speed-up includes test by caching the boundaries
                     * @param width
                     * @param height
                     */
                    inline void setBoundaries(const uint16_t width, const uint16_t height) {
                        _boundaries.x1 = _x >= 1 ? _x : _x * width;
                        _boundaries.y1 = _y >= 1 ? _y : _y * height;
                        _boundaries.x2 = (_w >= 1 ? _w : _w * width)  + _boundaries.x1;
                        _boundaries.y2 = (_h >= 1 ? _h : _h * height) + _boundaries.y1;
                    }

                    /**
                     * Get bounding box to speed calculation
                     * @param x1
                     * @param y1
                     * @param x2
                     * @param y2
                     */
                    void getBoundingBox(uint16_t* x1, uint16_t* y1, uint16_t* x2, uint16_t* y2) {
                        *x1 = _boundaries.x1;
                        *y1 = _boundaries.y1;
                        *x2 = _boundaries.x2;
                        *y2 = _boundaries.y2;
                    }

                    /**
                     * Test if RoI includes given point
                     * @param x
                     * @param y
                     * @return
                     */
                    bool includes(const uint16_t x, const uint16_t y) {
                        return
                            x >= _boundaries.x1
                            && x <= _boundaries.x2
                            && y >= _boundaries.y1
                            && y <= _boundaries.y2;
                    }

                    /**
                     *
                     * @return
                     */
                    String drawToCanvas() {
                        return String("ctx.strokeRect(")
                            + (_boundaries.x1 * 8)
                           + ", "
                           + (_boundaries.y1 * 8)
                           + ", "
                           + ((_boundaries.x2 - _boundaries.x1) * 8)
                           + ", "
                           + ((_boundaries.y2 - _boundaries.y1) * 8)
                           + ")";
                    }

                protected:
                    float _x;
                    float _y;
                    float _w;
                    float _h;
                    struct {
                        uint16_t x1;
                        uint16_t y1;
                        uint16_t x2;
                        uint16_t y2;
                    } _boundaries;
                };
            }
        }
    }
}


#endif //ELOQUENTESP32CAM_ROI_RECT_H
