//
// Created by Simone on 09/06/23.
//

#ifndef ELOQUENTEXTRA_BITMATRIX_H
#define ELOQUENTEXTRA_BITMATRIX_H


#include "./BitArray.h"


namespace Eloquent {
    namespace Extra {
        /**
         * Efficiently store bit matrices
         */
            class BitMatrix {
            public:
                BitArray array;
                size_t rows;
                size_t cols;

                /**
                 * Constructor
                 *
                 * @param arr
                 * @param numRows
                 * @param numCols
                 */
                BitMatrix(size_t numRows, size_t numCols) :
                array(numRows * numCols),
                rows(numRows),
                cols(numCols) {

                }

                /**
                 *
                 * @return
                 */
                inline size_t getRows() {
                    return rows;
                }

                /**
                 *
                 * @return
                 */
                inline size_t getCols() {
                    return cols;
                }

                /**
                 * Resize matrix
                 *
                 * @param numRows
                 * @param numCols
                 */
                bool resize(size_t numRows, size_t numCols) {
                    rows = numRows;
                    cols = numCols;

                    return array.resize(rows * cols);
                }

                /**
                 * Set bit at given row, col
                 */
                inline void set(size_t row, size_t col, bool value) {
                    array.set(row * cols + col, value);
                }

                /**
                 * Set bit at given index
                 */
                inline void set(size_t i, bool value) {
                    array.set(i, value);
                }

                /**
                 * Get bit at given row, col
                 */
                inline bool get(size_t row, size_t col) {
                    return get(row * cols + col);
                }

                /**
             * Get bit at given index
             */
                inline bool get(size_t idx) {
                    return array.get(idx);
                }

                /**
                 * Get number of true pixels
                 *
                 * @return
                 */
                inline size_t sum() {
                    return array.sum();
                }
            };
    }
}

#endif //ELOQUENTESP32CAM_BITMATRIX_H
