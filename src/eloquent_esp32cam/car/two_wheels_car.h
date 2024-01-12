#ifndef ELOQUENTESP32CAM_CAR_CAR_H
#define ELOQUENTESP32CAM_CAR_CAR_H

#include "./motor.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Car {
            /**
             * Control a 2-wheels car
             */
            class TwoWheelsCar {
            public:
                /**
                 * Constructor
                 * @param leftMotor
                 * @param rightMotor
                 */
                TwoWheelsCar(Motor& leftMotor, Motor& rightMotor) :
                        _left(&leftMotor),
                        _right(&rightMotor),
                        _defaultDuration(100) {

                }

                /**
                 * Don't actually actuate the motors
                 * (debug)
                 * @param pretend
                 */
                void pretend(bool pretend = true) {
                    _left->pretend(pretend);
                    _right->pretend(pretend);
                }

                /**
                 * How long should a motor action last
                 * by default?
                 * @param duration
                 */
                void defaultDuration(uint32_t duration) {
                    if (duration > 0)
                        _defaultDuration = duration;
                }

                /**
                 * Move forward
                 * @param duration
                 */
                void forward(uint32_t duration = 0) {
                    withTimeout([this]() {
                        ESP_LOGD("Car", "forward()");
                        _left->forward();
                        _right->forward();
                    }, duration);
                }

                /**
                 * Move forward forever
                 */
                void forwardForever() {
                    forward(0xFFFFFFFF);
                }

                /**
                 * Move backward
                 * @param duration
                 */
                void backward(uint32_t duration = 0) {
                    withTimeout([this]() {
                        ESP_LOGD("Car", "backward()");
                        _left->backward();
                        _right->backward();
                    }, duration);
                }

                /**
                 * Move backward forever
                 */
                void backwardForever() {
                    backward(0xFFFFFFFF);
                }

                /**
                 * Turn left
                 * @param duration
                 */
                void left(uint32_t duration = 0) {
                    withTimeout([this]() {
                        ESP_LOGD("Car", "left()");
                        _left->backward();
                        _right->forward();
                    }, duration);
                }

                /**
                 * Turn left forever
                 */
                void leftForever() {
                    left(0xFFFFFFFF);
                }

                /**
                 * Turn right
                 * @param duration
                 */
                void right(uint32_t duration = 0) {
                    withTimeout([this]() {
                        ESP_LOGD("Car", "right()");
                        _left->forward();
                        _right->backward();
                    }, duration);
                }

                /**
                 * Turn right forever
                 */
                void rightForever() {
                    right(0xFFFFFFFF);
                }

                /**
                 * Stop
                 * @param duration
                 */
                void stop(uint32_t duration = 0) {
                    _left->stop();
                    _right->stop();
                }

            protected:
                uint32_t _defaultDuration;
                Motor *_left, *_right;

                /**
                 * Actuate motors for given time, then stop
                 * @tparam Callback
                 * @param callback
                 * @param duration
                 */
                template<typename Callback>
                void withTimeout(Callback callback, uint32_t duration) {
                    callback();
                    delay(duration ? duration : _defaultDuration);
                    stop();
                }
            };
        }
    }
}

namespace eloq {
    namespace car {
        using TwoWheelsCar = Eloquent::Esp32cam::Car::TwoWheelsCar;
    }
}


#endif
