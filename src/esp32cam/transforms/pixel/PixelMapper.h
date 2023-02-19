//
// Created by Simone on 06/01/23.
//

#ifndef ELOQUENTESP32CAM_PIXELMAPPER_H
#define ELOQUENTESP32CAM_PIXELMAPPER_H

namespace Eloquent {
    namespace Esp32cam {
        namespace Transforms {
            namespace Pixel {

                /**
                 * Apply function to each pixel of the image
                 */
                class PixelMapper {
                public:
                    PixelMapper *next = NULL;

                    /**
                     *
                     * @param mapper
                     */
                    void chain(PixelMapper *mapper) {
                        if (next == NULL)
                            next = mapper;
                        else
                            next->chain(mapper);
                    }

                    /**
                     * Transform pixel
                     *
                     * @param channel
                     * @param i
                     * @param pixel
                     * @return
                     */
                    virtual uint8_t transform(uint8_t channel, size_t i, uint8_t pixel) = 0;
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_PIXELMAPPER_H
