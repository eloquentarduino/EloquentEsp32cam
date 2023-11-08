#ifndef ELOQUENT_EXTRA_CAR_CAR2WD_H
#define ELOQUENT_EXTRA_CAR_CAR2WD_H

#include "./motor.h"


namespace Eloquent {
    namespace Extra {
        namespace Car {
            class Car2WD {
                public:
                    /**
                     * 
                     */
                    Car2WD(Motor& left, Motor& right) :
                        _left(left),
                        _right(right) {

                        }

                    /**
                     * 
                     */
                    void begin() {
                        _left.begin();
                        _right.begin();
                    }

                    /**
                     * 
                     */
                    void forward() {
                        _left.forward();
                        _right.forward();
                    }

                    /**
                     * 
                     */
                    void backward() {
                        _left.backward();
                        _right.backward();
                    }

                    /**
                     * 
                     */
                    void stop() {
                        _left.stop();
                        _right.stop();
                    }

                    /**
                     * 
                     */
                    void left() {
                        _left.backward();
                        _right.forward();
                    }

                    /**
                     * 
                     */
                    void right() {
                        _left.forward();
                        _right.backward();
                    }

                protected:
                    Motor _left;
                    Motor _right;
            };
        }
    }
}


// sorter class
namespace eloq {
    typedef Eloquent::Extra::Car::Car2WD car_t;
}

#endif