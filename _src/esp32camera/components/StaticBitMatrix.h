//
// Created by Simone on 04/07/23.
//

#ifndef ELOQUENTESP32CAM_STATICBITMATRIX_H
#define ELOQUENTESP32CAM_STATICBITMATRIX_H

#include "./BitMatrix.h"
#include "./StaticBitArray.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Components {
            /**
             * Allocate bytes with static array
             * @tparam lengthInBits
             */
            template<size_t rows, size_t cols>
            class StaticBitMatrix : public BitMatrix {
            public:

                /**
                 *
                 */
                StaticBitMatrix() : BitMatrix(arr, rows, cols) {

                }

            protected:
                StaticBitArray<rows * cols> arr;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_STATICBITMATRIX_H
