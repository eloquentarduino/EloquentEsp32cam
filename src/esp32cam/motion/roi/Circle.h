//
// Created by Simone on 19/11/22.
//

#ifndef ELOQUENTESP32CAM_ROI_CIRCLE_H
#define ELOQUENTESP32CAM_ROI_CIRCLE_H

#include "./RoI.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            namespace RoI {
                class Circle : public RegionOfInterest {
                public:

                    /**
                     *
                     * @param x
                     * @param y
                     * @param radius
                     * @param treashold
                     */
                    Circle(float x, float y, float radius, float treashold = 0) {
                        setOrigin(x, y);
                        setRadius(radius);
                        setThreshold(treashold);
                    }

                    /**
                     * Typed constructor
                     * @tparam C
                     * @param definition
                     */
                    template<typename C>
                    Circle(C& definition) {
                        setOrigin(definition.x, definition.y);
                        setRadius(definition.radius);
                        setThreshold(definition.threshold);
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
                     * Set radius
                     * @param radius
                     */
                    inline void setRadius(float radius) {
                        _radius = radius / 8;
                        _radius2 = pow(_radius, 2);
                    }

                    /**
                     * Speed-up includes test by caching the boundaries
                     * @param width
                     * @param height
                     */
                    inline void setBoundaries(const uint16_t width, const uint16_t height) {
                        _origin.x = _x >= 1 ? _x : _x * width;
                        _origin.y = _y >= 1 ? _y : _y * height;
                    }

                    /**
                     * Get bounding box to speed calculation
                     * @param x1
                     * @param y1
                     * @param x2
                     * @param y2
                     */
                    void getBoundingBox(uint16_t* x1, uint16_t* y1, uint16_t* x2, uint16_t* y2) {
                        const uint16_t r = _radius;

                        *x1 = _origin.x > r ? _origin.x - r : 0;
                        *y1 = _origin.y > r ? _origin.y - r : 0;
                        *x2 = _origin.x + r;
                        *y2 = _origin.y + r;
                    }

                    /**
                     * Test if RoI includes given point
                     * @param x
                     * @param y
                     * @return
                     */
                    bool includes(const uint16_t x, const uint16_t y) {
                        const size_t dx = x > _origin.x ? x - _origin.x : _origin.x - x;
                        const size_t dy = x > _origin.y ? y - _origin.y : _origin.y - y;

                        return dx * dx + dy * dy <= _radius2;
                    }

                    /**
                     *
                     * @return
                     */
                    String drawToCanvas() {
                        return String("ctx.arc(")
                            + (_origin.x * 8)
                            + ", "
                            + (_origin.y * 8)
                            + ", "
                            + String(_radius * 8)
                            + ", 0, 2 * Math.PI, false); ctx.stroke();";
                    }

                protected:
                    float _x;
                    float _y;
                    float _radius;
                    float _radius2;
                    struct {
                        uint16_t x;
                        uint16_t y;
                    } _origin;
                };
            }
        }
    }
}


#endif //ELOQUENTESP32CAM_ROI_CIRCLE_H
