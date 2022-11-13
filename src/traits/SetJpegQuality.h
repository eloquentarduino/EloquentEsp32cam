//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_SETJPEGQUALITY_H
#define ELOQUENTESP32CAM_SETJPEGQUALITY_H

namespace Eloquent {
    namespace Esp32cam {
        /**
         * Configure quality of jpeg compression
         */
        class SetJpegQuality {
        public:

            /**
             * Set jpeg pixel format (low quality)
             */
            void lowQuality() {
                setQuality(30);
            }

            /**
             * Set jpeg pixel format (high quality)
             */
            void highQuality() {
                setQuality(20);
            }

            /**
             * Set jpeg pixel format (best quality)
             */
            void bestQuality() {
                setQuality(10);
            }

            /**
             * Set jpeg quality
             *
             * @param quality
             */
            void setQuality(uint8_t quality) {
                _jpegQuality = quality > 10 ? quality : 10;
            }

        protected:
            uint8_t _jpegQuality;
        };
    }
}

#endif //ELOQUENTESP32CAM_SETJPEGQUALITY_H
