#ifndef ELOQUENTES32CAM_BITARRAY_H
#define ELOQUENTES32CAM_BITARRAY_H


namespace Eloquent {
    namespace Esp32cam {
        namespace Components {

            /**
             * Efficiently store bit arrays
             */
            class BitArray {
            public:
                uint8_t *bytes;

                /**
                 * Constructor
                 */
                BitArray(size_t lengthInBits) {
                    _lengthInBits = lengthInBits;
                    resize(lengthInBits);
                }

                /**
                 *
                 * @return
                 */
                size_t length() {
                    return _lengthInBits;
                }

                /**
                 * Resize array
                 *
                 * @param lengthInBits
                 */
                bool resize(size_t lengthInBits) {
                    _lengthInBits = lengthInBits;
                    free(bytes);

                    if (lengthInBits) {
                        bytes = (uint8_t *) calloc((lengthInBits + 7) / 8, sizeof(uint8_t));

                        return bytes != NULL;
                    }
                    else {
                        bytes = NULL;

                        return true;
                    }
                }

                /**
                 * Set bit at given index
                 */
                void set(size_t idx, bool value) {
                    if (idx >= _lengthInBits || bytes == NULL)
                        return;

                    const size_t pos = idx / 8;
                    const uint8_t shift = idx % 8;
                    const uint8_t mask = (1 << shift);

                    if (value)
                        bytes[pos] |= mask;
                    else
                        bytes[pos] &= ~mask;
                }

                /**
                 * Get bit at given index
                 */
                bool get(size_t idx) {
                    if (idx >= _lengthInBits || bytes == NULL)
                        return false;

                    const uint16_t pos = idx / 8;
                    const uint8_t shift = idx % 8;
                    const uint8_t mask = (1 << shift);

                    return (bytes[pos] & mask) != 0;
                }

                /**
                 * Get number of true pixels
                 *
                 * @return
                 */
                size_t sum() {
                    size_t sum = 0;

                    for (size_t i = 0; i < _lengthInBits; i++)
                        if (get(i))
                            sum += 1;

                    return sum;
                }

            protected:
                size_t _lengthInBits;
            };

        }
    }
}

#endif