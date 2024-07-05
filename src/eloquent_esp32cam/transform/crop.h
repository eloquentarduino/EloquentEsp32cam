#ifndef ELOQUENT_ESP32CAM_TRANSFORM_CROP_H_
#define ELOQUENT_ESP32CAM_TRANSFORM_CROP_H_

#include <dl_image.hpp>
#include "../extra/exception.h"

using Eloquent::Error::Exception;
using namespace dl::image;


namespace Eloquent {
    namespace Esp32cam {
        namespace Transform {
            /**
             * Crop images
             */
            class Crop {
                public:
                    uint16_t *source;
                    Exception exception;

                    /**
                     * 
                     */
                    Crop() :
                        source(NULL),
                        exception("Crop"),
                        _lastSize(0),
                        _algo(IMAGE_RESIZE_NEAREST) {

                            rgb();
                            nearest();

                        }

                    /**
                     * Set src image
                     */
                    Crop& from(uint16_t* src, uint16_t width, uint16_t height) {
                        source = src;
                        _src.width = width;
                        _src.height = height;
                        _src.x1 = 0;
                        _src.y1 = 0;
                        _src.x2 = width;
                        _src.y2 = height;

                        return *this;
                    }

                    /**
                     * Set src image from frame
                     */
                    template<typename Frame>
                    Crop& from(Frame& frame) {
                        return from((uint16_t*) frame.data, frame.width, frame.height);
                    }

                    /**
                     * Set src image
                     * @param frame
                     * @return
                     */
                    Crop& from(camera_fb_t *frame) {
                        return from((uint16_t*) frame->buf, frame->width, frame->height);
                    }

                    /**
                     * 
                     */
                    Crop& to(uint16_t width, uint16_t height) {
                        _out.width = width;
                        _out.height = height;
                        _out.x1 = 0;
                        _out.y1 = 0;
                        _out.x2 = width;
                        _out.y2 = height;

                        return *this;
                    }

                    /**
                     * Keep colors
                     */
                    Crop& rgb(bool enabled = true) {
                        _out.channels = enabled ? 3 : 1;

                        return *this;
                    }

                    /**
                     * Convert to grayscale
                     */
                    Crop& gray() {
                        _out.channels = 1;

                        return *this;
                    }

                    /**
                     * Crop without preserving aspect ratio
                     */
                    Crop& squash() {
                        _src.x1 = 0;
                        _src.x2 = _src.width;
                        _src.y1 = 0;
                        _src.y2 = _src.height;

                        return offset(0, 0);
                    }

                    /**
                     * Center crop area
                     */
                    Crop& center() {
                        if (_src.width > _out.width) {
                            uint16_t dx = (_src.width - _out.width) / 2;
                            uint16_t dy = (_src.height - _out.height) / 2;

                            _src.x1 = dx;
                            _src.y1 = dy;
                            _src.x2 = _src.width - dx;
                            _src.y2 = _src.height - dy;

                            return offset(0, 0);
                        }
                        else if (_out.width > _src.width) {
                            uint16_t dx = (_out.width - _src.width) / 2;
                            uint16_t dy = (_out.height - _src.height) / 2;

                            _out.x1 = dx;
                            _out.y1 = dy;
                            _out.x2 = _out.width - dx;
                            _out.y2 = _out.height - dy;
                            _src.x1 = 0;
                            _src.y1 = 0;
                            _src.x2 = _src.width;
                            _src.y2 = _src.height;
                        }
                        
                        return *this;
                    }

                    /**
                     * Manually set crop area origin
                     * @param x
                     * @param y
                     * @return
                     */
                    Crop& offset(int16_t x, int16_t y) {
                        if (x < 0) x += _src.width;
                        if (y < 0) y += _src.height;

                        _src.x1 = x;
                        _src.x2 = x + _out.width;
                        _src.y1 = y;
                        _src.y2 = y + _out.height;

                        return *this;
                    }

                    /**
                     * No interpolation
                     */
                    Crop& nearest() {
                        _algo = IMAGE_RESIZE_NEAREST;

                        return *this;
                    }

                    /**
                     * Use mean interpolation 
                     */
                    Crop& linear() {
                        _algo = IMAGE_RESIZE_MEAN;

                        return *this;
                    }

                    /**
                     * Use bilinear interpolation 
                     */
                    Crop& bilinear() {
                        _algo = IMAGE_RESIZE_BILINEAR;

                        return *this;
                    }

                    /**
                     * Perform cropping
                     */
                    Exception& apply(uint8_t *out) {
                        if (!_out.height)
                            return exception.set("Output height not set");

                        if (!_out.width)
                            return exception.set("Output width not set");

                        if (!_src.height)
                            return exception.set("Source height not set");

                        if (!_src.width)
                            return exception.set("Source width not set");

                        if (_out.channels != 1 && _out.channels != 3)
                            return exception.set("Output channels must be 1 (gray) or 3 (rgb)");

                        ESP_LOGD("Crop", "src: w=%d, h=%d, y1=%d, y2=%d, x1=%d, x2=%d", _src.width, _src.height, _src.y1, _src.y2, _src.x1, _src.x2);
                        ESP_LOGD("Crop", "out: w=%d, h=%d, y1=%d, y2=%d, x1=%d, x2=%d", _out.width, _out.height, _out.y1, _out.y2, _out.x1, _out.x2);

                        crop_and_resize(
                            out,
                            _out.height,
                            _out.channels,
                            _out.y1,
                            _out.y2,
                            _out.x1,
                            _out.x2,
                            source,
                            _src.height,
                            _src.width,
                            3,
                            _src.y1,
                            _src.y2,
                            _src.x1,
                            _src.x2
                        );

                        return exception.clear();
                    }

                protected:
                    struct {
                        uint16_t width;
                        uint16_t height;
                        uint8_t channels;
                        uint16_t x1;
                        uint16_t x2;
                        uint16_t y1;
                        uint16_t y2;
                    } _src, _out;
                    uint16_t _lastSize;
                    resize_type_t _algo;
            };
        }
    }
}


namespace eloq {
    namespace cv {
        static Eloquent::Esp32cam::Transform::Crop crop;
    }
}

#endif