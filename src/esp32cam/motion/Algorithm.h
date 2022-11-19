//
// Created by Simone on 15/11/22.
//

#ifndef ELOQUENTESP32CAM_MOTION_ALGORITHM_H
#define ELOQUENTESP32CAM_MOTION_ALGORITHM_H

namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Abstract motion detection algorithm
             */
            class Algorithm {
            public:

                /**
                 * Initialize model
                 * @param pixels
                 * @param length
                 */
                virtual void initialize(uint8_t *pixels, size_t length) = 0;

                /**
                 * Train model
                 * @param pixels
                 * @param length
                 */
                virtual void train(uint8_t *pixels, size_t length) = 0;

                /**
                 * Receive new pixel
                 * @param offset
                 * @param pixel
                 * @param isForeground
                 */
                virtual void update(size_t offset, uint8_t pixel, bool isForeground) = 0;

                /**
                 * Test if pixel is foreground
                 * @param offset
                 * @param pixel
                 */
                virtual bool isForeground(size_t offset, uint8_t pixel) = 0;

                /**
                 * Set algorithm param by name
                 * @param param
                 * @param value
                 */
                virtual void set(const String& param, const float value) = 0;

                /**
                 * Get current configuration as string
                 * @return
                 */
                virtual String getCurrentConfig() = 0;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_MOTION_ALGORITHM_H
