#ifndef ELOQUENT_BITARRAY_H
#define ELOQUENT_BITARRAY_H


namespace Eloquent {
    namespace Esp32cam {
        namespace Components {
            /**
             * Efficiently store bit arrays
             */
            template<uint16_t lengthInBits>
            class BitArray {
            public:
                uint8_t bytes[(lengthInBits + 7) / 8];

                /**
                 * Constructor
                 */
                BitArray() {
                    memset(bytes, 0, (lengthInBits + 7) / 8);
                }

                /**
                 * Set bit at position
                 */
                void set(uint16_t idx, bool value) {
                    if (idx >= lengthInBits)
                        return;

                    const uint16_t pos = idx / 8;
                    const uint8_t shift = idx % 8;
                    const uint8_t mask = (1 << shift);

                    if (value)
                        bytes[pos] |= mask;
                    else
                        bytes[pos] &= ~mask;
                }

                /**
                 * Get bit at position
                 */
                bool get(uint16_t idx) {
                    if (idx >= lengthInBits)
                        return false;

                    const uint16_t pos = idx / 8;
                    const uint8_t shift = idx % 8;
                    const uint8_t mask = (1 << shift);

                    return (bytes[pos] & mask) != 0;
                }
            };
        }
    }
}

#endif