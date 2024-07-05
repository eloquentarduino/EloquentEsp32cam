#ifndef ELOQUENT_ESP32CAM_CAMERA_CONVERTER
#define ELOQUENT_ESP32CAM_CAMERA_CONVERTER

#include "../extra/exception.h"

using Eloquent::Error::Exception;


namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Convert frame between formats
             */
            template<typename Camera>
            class Converter565 {
                public:
                    Camera *camera;
                    Exception exception;
                    uint16_t *data;
                    size_t length;
                    size_t width;
                    size_t height;

                    /**
                     * 
                     */
                    Converter565(Camera *cam) : 
                        exception("RGB565Converter"), 
                        camera(cam),
                        length(0),
                        width(0),
                        height(0) {
                    }

                    /**
                     * Access data at given index
                     */
                    uint16_t operator[](size_t i) {
                        if (i >= length)
                            return 0;

                        if (data == NULL)
                            return 0;

                        return data[i];
                    }

                    /**
                     * Get RGB pixel at x, y
                     */
                    inline uint16_t at(size_t x, size_t y) {
                        return (*this)[y * width + x];
                    }

                    /**
                     * Get R or G or B pixel at x, y
                     */
                    inline uint8_t at(size_t x, size_t y, uint8_t channel) {
                        const uint16_t pixel = (*this)[y * width + x];

                        switch (channel) {
                            case 0:
                                return ((pixel >> 11) & 0x1F);
                            case 1:
                                return ((pixel >> 5) & 0x3F);
                            case 2:
                                return (pixel & 0x1F);
                            default:
                                ESP_LOGE("Converter565", "Channel %d is out of range (must be one of 0, 1 or 2)", channel);
                                return 0;
                        }
                    }

                    /**
                     * 
                     */
                    uint32_t as888(size_t i) {
                        uint32_t pixel = (*this)[i];

                        const int r = (pixel >> 11) & 0x1F;
                        const int g = (pixel >> 5) & 0x3F;
                        const int b = pixel & 0x1F;

                        return (r << 19) | (g << 10) | (b << 3);
                    }

                    /**
                     * Convert current frame to RGB565 format
                     * Use 8X scale down
                     */
                    Exception& convert() {
                        if (!camera->hasFrame())
                            return exception.set("Can't convert empty frame to RGB565");

                        if (!width) {
                            width = camera->resolution.getWidth() / 8;
                            height = camera->resolution.getHeight() / 8;
                            length = width * height;

                            ESP_LOGI("Camera", "Allocating %d bytes for %dx%d RGB565 image", length * 2, width, height);
                            data = (uint16_t*) ps_malloc(length * 2);
                        }

                        if (data == NULL)
                            return exception.set("Cannot allocate memory");

                        camera->mutex.threadsafe([this]() {
                            if (!jpg2rgb565(camera->frame->buf, camera->frame->len, (uint8_t*) data, JPG_SCALE_8X))
                                exception.set("Error converting frame from JPEG to RGB565");
                        });

                        if (!exception.isOk())
                            return exception;
                        
                        swapBytes();

                        return exception.clear();
                    }

                    /**
                     * With some boards, jpg2rgb565 swaps the bytes.
                     * Use this function to revert the swap
                     */
                    void swapBytes() {
                        for (size_t i = 0; i < length; i++) {
                            const uint16_t pixel = data[i];
                            const uint16_t h = (pixel >> 8);
                            const uint16_t l = pixel & 0xFF;

                            data[i] = (l << 8) | h;
                        }
                    }
            };
        }
    }
}

#endif