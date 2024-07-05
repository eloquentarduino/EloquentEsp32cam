#ifndef ELOQUENT_ESP32CAM_MOTION_ROI_DETECTION
#define ELOQUENT_ESP32CAM_MOTION_ROI_DETECTION

#include "./detection.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Perform motion detection on Region of Interest
             */
            class RoI : public Detection {
                public:
                    struct {
                        uint16_t x;
                        uint16_t y;
                        uint16_t width;
                        uint16_t height;
                        uint16_t x1;
                        uint16_t x2;
                        uint16_t y1;
                        uint16_t y2;
                    } coords;

                    /**
                     * 
                     */
                    RoI() :
                        _x(0),
                        _y(0),
                        _w(0),
                        _h(0) {

                        }

                    /**
                     * Set x coordinate (top-left corner)
                     */
                    void x(float x) {
                        _x = x;
                    }

                    /**
                     * Set y coordinate (top-left corner)
                     */
                    void y(float y) {
                        _y = y;
                    }

                    /**
                     * Set width of RoI
                     */
                    void width(float width) {
                        _w = width;
                    }

                    /**
                     * Set height of RoI
                     */
                    void height(float height) {
                        _h = height;
                    }

                    /**
                     * 
                     */
                    void updateCoords(uint16_t width, uint16_t height) {
                        coords.x = max<int>(0, _x < 1 ? _x * width : _x);
                        coords.y = max<int>(0, _y < 1 ? _y * height : _y);
                        coords.width = min<int>(width - coords.x, _w < 1 ? _w * width : _w);
                        coords.height = min<int>(height - coords.y, _h < 1 ? _h * height : _h);
                        coords.x1 = coords.x;
                        coords.y1 = coords.y;
                        coords.x2 = coords.x + coords.width;
                        coords.y2 = coords.y + coords.height;
                    }

                    /**
                     * Detect motion
                     */
                    template<typename Frame>
                    Exception& update(Frame& frame) {
                        if (!_w || !_h)
                            return exception.set("You MUST set a width and height for the RoI");

                        if (_prev == NULL) {
                            _prev = (uint8_t*) ps_malloc(_w * _h * sizeof(uint16_t));
                            _roi  = (uint8_t*) ps_malloc(_w * _h * sizeof(uint16_t));
                            copy(frame, _prev);

                            return exception.set("First frame, can't detect motion").soft();
                        }

                        if (!rate_limiter) {
                            // update prev, but don't run prediction
                            copy(frame, _prev);

                            return exception.set(rate_limiter.getRetryInMessage()).soft();
                        }

                        updateCoords(frame.width, frame.height);

                        benchmark.timeit([this, &frame]() {
                            copy(frame, _roi);

                            int movingPoints = dl::image::get_moving_point_number((uint16_t *) _roi, (uint16_t*) _prev, coords.height, coords.width, _stride, _threshold);
                            moving_ratio = ((float) movingPoints) / sizeof(_roi) * _stride * _stride;
                            memcpy(_prev, _roi, sizeof(_prev));
                        });
                        
                        ESP_LOGD(
                            "RoI MotionDetection", 
                            "roi: (x=%d, y=%d, width=%d, height=%d). moving points: %.2f%%", 
                            coords.x,
                            coords.y,
                            coords.width,
                            coords.height,
                            moving_ratio
                        );

                        if (triggered())
                            rate_limiter.touch();

                        return exception.clear();
                    }


                protected:
                    float _x;
                    float _y;
                    float _w;
                    float _h;
                    uint8_t *_roi;

                    /**
                     * Copy RoI of frame into buffer
                     */
                    template<typename Frame>
                    void copy(Frame& frame, uint8_t *dest) {
                        for (int i = coords.y1; i < coords.y2; i++)
                            memcpy(
                                dest + coords.width * (i - coords.y1) * sizeof(uint16_t), 
                                frame.data + (frame.width * i + coords.x) * sizeof(uint16_t), 
                                coords.width * sizeof(uint16_t)
                            );
                    }
            };
        }
    }
}

namespace eloq {
    namespace motion {
        // create class alias
        class RoI : public Eloquent::Esp32cam::Motion::RoI {};
    }
}

#endif