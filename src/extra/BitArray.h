#ifndef ELOQUENTEEXTRA_BITARRAY_H
#define ELOQUENTEEXTRA_BITARRAY_H


#include "./ascii.h"


namespace Eloquent {
    namespace Extra {

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
                ESP_LOGI("BitArray", "Allocating %lu bits", (unsigned long) lengthInBits);

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
                const uint16_t lengthInBytes = _lengthInBits / 8;
                size_t sum = 0;

                for (size_t i = 0; i < lengthInBytes; i += 4) {
                    uint32_t tmp;

                    memcpy(&tmp, bytes + i, min(sizeof(tmp), lengthInBytes - i));

                    // Brian Kernighan’s Algorithm
                    while (tmp) {
                        tmp &= (tmp - 1);
                        sum += 1;
                    }
                }

                // for (size_t i = 0; i < _lengthInBits; i++)
                //     if (get(i))
                //         sum += 1;

                return sum;
            }

            /**
             * Efficient iterator
             */
            template<typename Iterator>
            void forEach(Iterator iterator) {
                const uint16_t lengthInBytes = _lengthInBits / 8;
                size_t i = 0;

                for (uint16_t byteIdx = 0; byteIdx < lengthInBytes; byteIdx++) {
                    const uint8_t byte = bytes[byteIdx];

                    for (int8_t shift = 7; shift >= 0; shift--) {
                        bool bit = (byte >> shift) & 0b1;

                        iterator(i++, bit);

                        if (i >= _lengthInBits)
                            return;
                    }
                }
            }

            /**
             * Efficient set
             */
            template<typename Setter>
            void setEach(Setter setter) {
                const uint16_t lengthInBytes = _lengthInBits / 8;
                size_t i = 0;

                for (uint16_t byteIdx = 0; byteIdx < lengthInBytes; byteIdx++) {
                    for (int8_t shift = 7; shift >= 0; shift--) {
                        const bool bit = setter(i++);
                        const uint8_t mask = (1 << shift);

                        if (bit)
                            bytes[byteIdx] |= mask;
                        else
                            bytes[byteIdx] &= ~mask;

                        if (i >= _lengthInBits)
                            return;
                    }
                }
            }

            /**
             * Convert to (simplified) ASCII string
             */
            String toASCII() {
                String ascii = "";

                ascii.reserve(_lengthInBits / 6);

                const uint16_t lengthInBytes = _lengthInBits / 8;
                size_t i = 0;

                for (uint16_t byteIdx = 0; byteIdx < lengthInBytes; byteIdx += 3) {
                    const uint16_t a = bytes[byteIdx];
                    const uint16_t b = byteIdx < lengthInBytes - 1 ? bytes[byteIdx + 1] : 0;
                    const uint16_t c = byteIdx < lengthInBytes - 2 ? bytes[byteIdx + 2] : 0;
                    const uint16_t ab = (a << 8) | b;
                    const uint16_t bc = (b << 8) | c;

                    const uint8_t i = (a & 0b11111100) >> 2;
                    const uint8_t j = (ab & 0b1111110000) >> 4;
                    const uint8_t k = (bc & 0b111111000000) >> 6;
                    const uint8_t l = c & 0b111111;

                    ascii += SIMPLIFIED_ASCII_ALPHABET[i];
                    ascii += SIMPLIFIED_ASCII_ALPHABET[j];
                    ascii += SIMPLIFIED_ASCII_ALPHABET[k];
                    ascii += SIMPLIFIED_ASCII_ALPHABET[l];
                }

                return ascii;
            }

        protected:
            size_t _lengthInBits;
        };

    }
}

#endif