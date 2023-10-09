#ifndef ELOQUENT_ESP32CAM_CAMERA_JPEGQUALITY
#define ELOQUENT_ESP32CAM_CAMERA_JPEGQUALITY


namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Configure quality of jpeg compression
             */
            class JpegQuality {
            public:
                uint8_t quality;

                /**
                 * Constructor
                 */
                JpegQuality() {
                    high();
                }

                /**
                 * Quality = 64
                 */
                void worst() {
                    set(64);
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
                    quality = constrain(q, 10, 64);
                }
            };
        }
    }
}

#endif
