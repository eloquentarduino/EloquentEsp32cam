#ifndef ELOQUENTESP32CAM_CAR_MOTOR_H
#define ELOQUENTESP32CAM_CAR_MOTOR_H


namespace Eloquent {
    namespace Esp32cam {
        namespace Car {
            /**
             * Control a single motor
             */
            class Motor {
            public:
                /**
                 * Constructor
                 * @param pin1
                 * @param pin2
                 */
                Motor(uint8_t pin1, uint8_t pin2) :
                        _a(pin1),
                        _b(pin2),
                        _positive(true),
                        _pretend(false) {

                }

                /**
                 * Don't actually actuate the motor
                 * (debug)
                 * @param pretend
                 */
                void pretend(bool pretend = true) {
                    _pretend = pretend;
                }

                /**
                 * Reverse motor polarity
                 */
                void reverse() {
                    _positive = !_positive;
                }

                /**
                 * Move forward
                 */
                void forward() {
                    write(_positive, !_positive);
                }

                /**
                 * Move backward
                 */
                void backward() {
                    write(!_positive, _positive);
                }

                /**
                 * Stop
                 */
                void stop() {
                    write(LOW, LOW);
                }

            protected:
                uint8_t _a;
                uint8_t _b;
                bool _positive;
                bool _pretend;

                /**
                 *
                 * @param dirA
                 * @param dirB
                 */
                void write(bool dirA, bool dirB) {
                    ESP_LOGV("Motor", "write(%d=%d, %d=%d)%s", _a, dirA, _b, dirB, _pretend ? " (pretended)" : "");

                    if (!_pretend) {
                        pinMode(_a, OUTPUT);
                        pinMode(_b, OUTPUT);
                        digitalWrite(_a, dirA);
                        digitalWrite(_b, dirB);
                    }
                }
            };
        }
    }
}

namespace eloq {
    namespace car {
        using Motor = Eloquent::Esp32cam::Car::Motor;
    }
}

#endif