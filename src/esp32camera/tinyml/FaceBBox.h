//
// Created by Simone on 03/07/23.
//

#ifndef ELOQUENTESP32CAM_FACEBBOX_H
#define ELOQUENTESP32CAM_FACEBBOX_H

namespace Eloquent {
    namespace Esp32cam {
        namespace TinyML {
            /**
             * Face bounding box
             */
            class FaceBBox {
            public:
                uint16_t left;
                uint16_t right;
                uint16_t top;
                uint16_t bottom;
                uint16_t width;
                uint16_t height;
                uint16_t x;
                uint16_t y;
                uint16_t cx;
                uint16_t cy;

                /**
                 * Convert to JSON
                 *
                 * @return
                 */
                String toJson() {
                    return String("{\"left\":")
                           + left
                           + ", \"right\":"
                           + right
                           + ", \"top\":"
                           + top
                           + ", \"bottom\":"
                           + bottom
                           + "}";
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_FACEBBOX_H
