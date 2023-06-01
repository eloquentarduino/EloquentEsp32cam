//
// Created by Simone on 09/06/23.
//

#ifndef ELOQUENTESP32CAM_BITMATRIX_H
#define ELOQUENTESP32CAM_BITMATRIX_H


#include "./BitArray.h"


namespace Eloquent {
    namespace Esp32Cam {
        namespace Components {
            /**
             * Efficiently store bit matrices
             */
             template<uint16_t rows, uint16_t cols>
             class BitMatrix {
             public:
                 BitArray<rows * cols> array;

                 /**
                 * Set bit at given row, col
                 */
                 inline void set(uint16_t row, uint16_t col, bool value) {
                     array.set(row * cols + col, value);
                 }

                 /**
                 * Get bit at given row, col
                 */
                 inline bool get(uint16_t row, uint16_t col) {
                     return get(row * cols + col);
                 }

                 /**
                 * Get bit at given index
                 */
                 inline bool get(uint16_t idx) {
                     return array.get(idx);
                 }
             };
        }
    }
}

#endif //ELOQUENTESP32CAM_BITMATRIX_H
