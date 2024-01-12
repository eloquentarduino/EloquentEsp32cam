#ifndef ELOQUENTESP32CAM_CAR_FOMO_DRIVEN_CAR_H
#define ELOQUENTESP32CAM_CAR_FOMO_DRIVEN_CAR_H

#ifndef EI_CLASSIFIER_INPUT_WIDTH
#error "You must include a Edge Impulse library"
#else

#include "../edgeimpulse/fomo.h"
#include "./two_wheels_car.h"

using eloq::ei::bbox_t;
using eloq::ei::fomo;
using Eloquent::Esp32cam::EdgeImpulse::FOMO;


namespace Eloquent {
    namespace Esp32cam {
        namespace Car {
            /**
             * Autonomous car that follows a bounding box
             */
            class FomoDrivenCar : public TwoWheelsCar {
            public:

                /**
                 * Constructor
                 * @param leftMotor
                 * @param rightMotor
                 */
                FomoDrivenCar(Motor& leftMotor, Motor& rightMotor) :
                    TwoWheelsCar(leftMotor, rightMotor),
                    _reversed(false) {

                    _exploration.last = 0;
                    _exploration.state = 0;
                }

                /**
                 * Allows to accomodate camera rotation
                 * (horizontal, top-down)
                 */
                void rotate(uint16_t angle) {
                    if (angle != 0 && angle != 90 && angle != 180 && angle != 270) {
                        ESP_LOGE("FomoDrivenCar", "Invalid rotation: %d. Only 0, 90, 180, 270 are allowed", angle);
                        return;
                    }

                    _rotation = angle;
                }

                /**
                 *
                 */
                void reverse(bool reverse = true) {
                    _reversed = reverse;
                }

                /**
                 *
                 * @param fomo
                 */
                void follow(FOMO& fomo) {
                    if (!fomo.found()) {
                        ESP_LOGD("FomoDrivenCar", "No object found");
                        _explore();
                        return;
                    }

                    uint16_t cx = 0;
                    uint16_t dimension = 0;

                    switch (_rotation) {
                        case 0:
                            cx = fomo.first.x + fomo.first.width / 2;
                            dimension = EI_CLASSIFIER_INPUT_WIDTH;
                            break;
                        case 180:
                            cx = EI_CLASSIFIER_INPUT_WIDTH - fomo.first.x - fomo.first.width / 2;
                            dimension = EI_CLASSIFIER_INPUT_WIDTH;
                            break;
                        case  90:
                            // todo
                            break;
                        case 270:
                            // todo
                            break;
                    }

                    ESP_LOGD("FomoDrivenCar", "bbox.x=%d, bbox.w=%d, cx=%d, dimension=%d", fomo.first.x, fomo.first.width, cx, dimension);

                    const float band = dimension / 6;

                    if (cx <= band * 2)
                        _reversed ? left() : right();
                    else if (cx >= band * 4)
                        _reversed ? right() : left();

                    forward(_defaultDuration * 2);

                    _exploration.last = millis();
                    _exploration.state = 0;
                }

            protected:
                bool _reversed;
                uint16_t _rotation = 0;
                struct {
                    size_t last;
                    uint8_t state;
                } _exploration;

                /**
                 * If no object is detected, look around
                 */
                void _explore() {
                    // explore only if no object is detected for X seconds
                    if (millis() - _exploration.last < 2000)
                        return;

                    switch (_exploration.state) {
                        case 0:
                            left(50);
                            break;
                        case 1:
                            right(100);
                            break;
                        case 2:
                            left(150);
                            break;
                        case 3:
                            right(200);
                            break;
                        case 4:
                            left(250);
                            break;
                        case 5:
                            right(300);
                            break;
                    }

                    _exploration.last = millis() - 1000;
                    _exploration.state = (_exploration.state + 1) % 6;
                }
            };
        }
    }
}

namespace eloq {
    namespace car {
        using Car = Eloquent::Esp32cam::Car::FomoDrivenCar;
    }
}

#endif
#endif
