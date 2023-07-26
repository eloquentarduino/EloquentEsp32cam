/**
 * This code is taken from https://github.com/suyash/ulid/tree/master
 * Their license applies
 */
#ifndef ELOQUENT_EXTRA_ULID
#define ELOQUENT_EXTRA_ULID


namespace Eloquent {
    namespace Extra {
        /**
         * Generate ULIDs (sortable UIDs)
         */
        class ULID {
            public:
                uint8_t data[16];

                /**
                 * Constructor
                 */
                ULID(uint32_t timestamp, uint8_t entropy) {
                    memset(data, 0, 16);
                    encodeTime(timestamp);
                    encodeEntropy(entropy);
                }

                /**
                 * Convert to string
                 */
                String toString() {
                    static const char alphabet[33] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
                    char dst[27] = {'\0'};

                    // 10 byte timestamp
                    dst[0] = alphabet[(data[0] & 224) >> 5];
                    dst[1] = alphabet[data[0] & 31];
                    dst[2] = alphabet[(data[1] & 248) >> 3];
                    dst[3] = alphabet[((data[1] & 7) << 2) | ((data[2] & 192) >> 6)];
                    dst[4] = alphabet[(data[2] & 62) >> 1];
                    dst[5] = alphabet[((data[2] & 1) << 4) | ((data[3] & 240) >> 4)];
                    dst[6] = alphabet[((data[3] & 15) << 1) | ((data[4] & 128) >> 7)];
                    dst[7] = alphabet[(data[4] & 124) >> 2];
                    dst[8] = alphabet[((data[4] & 3) << 3) | ((data[5] & 224) >> 5)];
                    dst[9] = alphabet[data[5] & 31];

                    // 16 bytes of entropy
                    dst[10] = alphabet[(data[6] & 248) >> 3];
                    dst[11] = alphabet[((data[6] & 7) << 2) | ((data[7] & 192) >> 6)];
                    dst[12] = alphabet[(data[7] & 62) >> 1];
                    dst[13] = alphabet[((data[7] & 1) << 4) | ((data[8] & 240) >> 4)];
                    dst[14] = alphabet[((data[8] & 15) << 1) | ((data[9] & 128) >> 7)];
                    dst[15] = alphabet[(data[9] & 124) >> 2];
                    dst[16] = alphabet[((data[9] & 3) << 3) | ((data[10] & 224) >> 5)];
                    dst[17] = alphabet[data[10] & 31];
                    dst[18] = alphabet[(data[11] & 248) >> 3];
                    dst[19] = alphabet[((data[11] & 7) << 2) | ((data[12] & 192) >> 6)];
                    dst[20] = alphabet[(data[12] & 62) >> 1];
                    dst[21] = alphabet[((data[12] & 1) << 4) | ((data[13] & 240) >> 4)];
                    dst[22] = alphabet[((data[13] & 15) << 1) | ((data[14] & 128) >> 7)];
                    dst[23] = alphabet[(data[14] & 124) >> 2];
                    dst[24] = alphabet[((data[14] & 3) << 3) | ((data[15] & 224) >> 5)];
                    dst[25] = alphabet[data[15] & 31];

                    return String(dst + 2);
                }

            protected:

                /**
                 * Encode time in the first 6 bytes
                 * timestamp
                 * */
                inline void encodeTime(uint32_t timestamp) {
                    data[0] = 0;
                    data[1] = 0;
                    data[2] = static_cast<uint8_t>(timestamp >> 24);
                    data[3] = static_cast<uint8_t>(timestamp >> 16);
                    data[4] = static_cast<uint8_t>(timestamp >> 8);
                    data[5] = static_cast<uint8_t>(timestamp);
                }

                /**
                 * Encode entropy from given random seed
                 */
                void encodeEntropy(uint8_t seed) {
                    randomSeed(seed);

                    data[6] = random(255);
                    data[7] = random(255);
                    data[8] = random(255);
                    data[9] = random(255);
                    data[10] = random(255);
                    data[11] = random(255);
                    data[12] = random(255);
                    data[13] = random(255);
                    data[14] = random(255);
                    data[15] = random(255);
                }
        };
    }
}

#endif