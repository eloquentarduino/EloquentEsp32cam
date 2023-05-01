#ifndef ELOQUENTESP32CAM_SETJPEGQUALITY_H
#define ELOQUENTESP32CAM_SETJPEGQUALITY_H


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Configure quality of jpeg compression
             */
            class SetsJpegQuality {
            public:
                uint8_t quality;

                /**
                 * Constructor
                 */
                SetsJpegQuality() {
                    high();
                }


                /**
                 * Quality = 30
                 */
                void low() {
                    set(30);
                }

                /**
                 * Quality = 20
                 */
                void high() {
                    set(20);
                }

                /**
                 * Quality = 10
                 */
                void best() {
                    set(10);
                }

                /**
                 * Set jpeg quality
                 *
                 * @param q
                 */
                void set(uint8_t q) {
                    quality = q > 10 ? q : 10;
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SETJPEGQUALITY_H
