#ifndef ELOQUENT_ESP32CAM_MOTION_DETECTION
#define ELOQUENT_ESP32CAM_MOTION_DETECTION

#include <dl_image.hpp>
#include "../extra/exception.h"
#include "../extra/time/benchmark.h"
#include "../extra/time/rate_limit.h"
#include "../extra/pubsub.h"
#include "./daemon.h"

using eloq::camera;
using Eloquent::Error::Exception;
using Eloquent::Extra::Time::Benchmark;
using Eloquent::Extra::Time::RateLimit;
#if defined(ELOQUENT_EXTRA_PUBSUB_H)
using Eloquent::Extra::PubSub;
#endif


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Detect motion using "fast" algorithm
             */
            class Detection {
                public:
                    float movingRatio;
                    Exception exception;
                    Benchmark benchmark;
                    RateLimit rate;
                    Daemon<Detection> daemon;
                    #if defined(ELOQUENT_EXTRA_PUBSUB_H)
                    PubSub<Detection> mqtt;
                    #endif
                    
                    /**
                     * 
                     */
                    Detection() :
                        _stride(4),
                        _threshold(5),
                        _ratio(0.2),
                        _prev(NULL),
                        _skip(5),
                        movingRatio(0),
                        daemon(this),
                        #if defined(ELOQUENT_EXTRA_PUBSUB_H)
                        mqtt(this),
                        #endif
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
                     * @brief Skip first frames (to avoid false detection)
                     * @param skip
                     */
                    void skip(uint8_t skip) {
                        _skip = skip;
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
                        return movingRatio >= _ratio;
                    }

                    /**
                     * 
                     */
                    Exception& run() {
                        // skip fre first frames
                        if (_skip > 0 && _skip-- > 0)
                            return exception.set(String("Still ") + _skip + " frames to skip...");
                            
                        // convert JPEG to RGB565
                        // this reduces the frame to 1/8th
                        if (!camera.rgb565.convert().isOk())
                            return camera.rgb565.exception;

                        // first frame, only copy frame to prev
                        if (_prev == NULL) {
                            _prev = (uint16_t*) ps_malloc(camera.rgb565.length * sizeof(uint16_t));
                            copy(camera.rgb565);

                            return exception.set("First frame, can't detect motion").soft();
                        }

                        benchmark.timeit([this]() {
                            int movingPoints = dl::image::get_moving_point_number(
                                camera.rgb565.data, 
                                _prev, 
                                camera.rgb565.height, 
                                camera.rgb565.width, 
                                _stride, 
                                _threshold
                            );

                            movingRatio = ((float) movingPoints) / camera.rgb565.length * _stride * _stride;
                            copy(camera.rgb565);
                        });

                        ESP_LOGD("MotionDetection", "moving points ratio: %.2f", movingRatio);

                        // rate limit
                        if (triggered() && !rate)
                            return exception.set(rate.getRetryInMessage()).soft();

                        if (triggered())
                            rate.touch();

                        return exception.clear();
                    }
                    
                    /**
                     * @brief Convert to JSON
                     */
                    String toJSON() {
                        return String("{\"motion\":") + (triggered() ? "true" : "false") + "}";
                    }
                    
                    /**
                     * @brief Test if an MQTT message should be published
                     */
                    bool shouldPub() {
                        return triggered();
                    }

                protected:
                    uint8_t _skip;
                    uint16_t *_prev;
                    uint8_t _stride;
                    uint8_t _threshold;
                    float _ratio;

                    /**
                     * 
                     */
                    template<typename Frame>
                    void copy(Frame frame) {
                        memcpy((uint8_t*) _prev, (uint8_t*) frame.data, frame.length * sizeof(uint16_t));
                    }
            };
        }
    }
}

namespace eloq {
    namespace motion {
        static Eloquent::Esp32cam::Motion::Detection detection;
    }
}

#endif