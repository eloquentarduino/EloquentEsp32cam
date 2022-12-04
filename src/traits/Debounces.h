//
// Created by Simone on 11/09/22.
//

#ifndef ELOQUENTESP32CAM_DEBOUNCES_H
#define ELOQUENTESP32CAM_DEBOUNCES_H


namespace Eloquent {
    namespace Esp32cam {
        /**
         * Debounce actions
         */
        class Debounces {
        public:

            /**
             * Set debounce interval
             *
             * @param value
             */
            void debounce(size_t value) {
                _debounce = value;
            }

            /**
             * Set debounce interval
             *
             * @param value
             */
            void debounceSeconds(size_t value) {
                _debounce = value * 1000;
            }

        protected:
            size_t _lastTick = 0;
            size_t _debounce = 0;

            /**
             * Get time elapsed since last event
             *
             * @return
             */
            inline size_t elapsed() {
                return millis() - _lastTick;
            }

            /**
             * Test if debounce time elapsed
             *
             * @return
             */
            inline bool debounced() {
                // check for overflow!
                return _lastTick == 0 || millis() < _lastTick || elapsed() > _debounce;
            }

            /**
             * Update last tick
             * @return true
             */
            inline bool touch() {
                _lastTick = millis();

                return true;
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_DEBOUNCES_H
