//
// Created by Simone on 04/07/23.
//

#ifndef ELOQUENTESP32CAM_STATICBITARRAY_H
#define ELOQUENTESP32CAM_STATICBITARRAY_H

#include "./BitArray.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Components {
            /**
             * Allocate bytes with static array
             * @tparam lengthInBits
             */
            template<size_t lengthInBits>
            class StaticBitArray : public BitArray {
            public:

                /**
                 *
                 */
                StaticBitArray() : BitArray(buffer, lengthInBits) {

                }

            protected:
                uint8_t buffer[(lengthInBits + 7) / 8];
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_STATICBITARRAY_H
