#ifndef ELOQUENT_ESP32CAM_MOTION_DETECTION
#define ELOQUENT_ESP32CAM_MOTION_DETECTION

#include <dl_image.hpp>
#include "../extra/exception.h"
#include "../extra/time/rate_limit.h"

using Eloquent::Extra::Exception;
using Eloquent::Extra::Time::RateLimit;


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Detect motion using "fast" algorithm
             */
            class Detection {
                public:
                    float moving_ratio;
                    Exception exception;
                    RateLimit rate_limiter;
                    
                    /**
                     * 
                     */
                    Detection() :
                        _stride(4),
                        _threshold(5),
                        _ratio(0.2),
                        _prev(NULL),
                        moving_ratio(0),
                        exception("MotionDetection") {

                        }

                    /**
                     * Set detection stride.
                     * The greater the value, the less accurate.
                     * The greater the value, the faster.
                    */
                    void stride(uint8_t stride) {
                        _stride = stride;
                    }

                    /**
                     * Set detection sensitivity (pixel level).
                     * The greater the value, the less sensitive the detection.
                     */
                    void threshold(uint8_t threshold) {
                        _threshold = threshold;
                    }

                    /**
                     * Set detection sensitivity (image level).
                     * The greater the value, the less sensitive the detection.
                     */
                    void ratio(float ratio) {
                        if (ratio <= 0 || ratio > 1) {
                            ESP_LOGE("MotionDetection", "ratio MUST be between 0 (exclusive) and 1 (inclusive)");
                            return;
                        }

                        _ratio = ratio;
                    }

                    /**
                     * Test if motion triggered
                     */
                    inline bool triggered() {
                        return moving_ratio >= _ratio;
                    }

                    /**
                     * 
                     */
                    template<typename Frame>
                    Exception& update(Frame& frame) {
                        if (_prev == NULL) {
                            _prev = (uint8_t*) malloc(frame.length);
                            memcpy(_prev, frame.data, frame.length);

                            return exception.set("First frame, can't detect motion").soft();
                        }

                        if (!rate_limiter) {
                            // update prev, but don't run prediction
                            memcpy(_prev, frame.data, frame.length);

                            return exception.set(rate_limiter.getRetryInMessage());
                        }

                        int movingPoints = dl::image::get_moving_point_number((uint16_t *) frame.data, (uint16_t*) _prev, frame.height, frame.width, _stride, _threshold);
                        moving_ratio = ((float) movingPoints) / frame.length * _stride * _stride;
                        memcpy(_prev, frame.data, frame.length);
                        ESP_LOGD("MotionDetection", "moving points: %d (%.2f)", movingPoints, movingRatio);

                        if (triggered())
                            rate_limiter.touch();

                        return exception.clear();
                    }

                protected:
                    uint8_t *_prev;
                    uint8_t _stride;
                    uint8_t _threshold;
                    float _ratio;
            };
        }
    }
}

namespace e {
    static Eloquent::Esp32cam::Motion::Detection motion_detection;
}

#endif