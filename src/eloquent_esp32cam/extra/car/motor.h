#ifndef ELOQUENT_EXTRA_CAR_MOTOR_H
#define ELOQUENT_EXTRA_CAR_MOTOR_H

namespace Eloquent {
    namespace Extra {
        namespace Car {
            class Motor {
                public:
                    /**
                     * 
                     */
                    Motor(uint8_t fwd, uint8_t rev) :
                        _fwd(fwd),
                        _rev(rev) {

                        }

                    /**
                     * 
                     */
                    void begin() {
                        pinMode(_fwd, OUTPUT);
                        pinMode(_rev, OUTPUT);
                    }

                    /**
                     * 
                     */
                    void forward() {
                        digitalWrite(_fwd, 1);
                        digitalWrite(_rev, 0);
                    }

                    /**
                     * 
                     */
                    void backward() {
                        digitalWrite(_fwd, 0);
                        digitalWrite(_rev, 1);
                    }

                    /**
                     * 
                     */
                    void stop() {
                        digitalWrite(_fwd, 0);
                        digitalWrite(_rev, 0);
                    }

                protected:
                    uint8_t _fwd;
                    uint8_t _rev;
            };
        }
    }
}

// shorter class
namespace eloq {
    typedef Eloquent::Extra::Car::Motor motor_t;
}

#endif