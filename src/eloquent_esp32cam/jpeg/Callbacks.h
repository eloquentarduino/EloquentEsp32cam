#ifndef ELOQUENTESP32CAM_JPEG_CALLBACKS
#define ELOQUENTESP32CAM_JPEG_CALLBACKS

#include <functional>

namespace Eloquent {
    namespace Esp32cam {
        namespace JPEG {
            /**
             * Callbacks for JPEGDECWrapper
             */
             template<typename Callback, uint8_t numCallbacks>
            class Callbacks {
            public:

                /**
                 * Constructor
                 */
                Callbacks() : _i(0) {

                }

                /**
                 * Add callback
                 * @param callback
                 * @return
                 */
                bool add(Callback callback) {
                    if (_i == numCallbacks)
                        return false;

                    _callbacks[_i++] = callback;
                    return true;
                }

                /**
                 * Loop over callbacks
                 * @param fn
                 */
                 template<typename ForEach>
                void forEach(ForEach fn) {
                    for (uint8_t i = 0; i < _i; i++)
                        fn(_callbacks[i]);
                }

            protected:
                uint8_t _i;
                Callback _callbacks[numCallbacks];
            };
        }
    }
}

#endif