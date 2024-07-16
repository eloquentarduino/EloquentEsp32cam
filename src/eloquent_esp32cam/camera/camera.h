#ifndef ELOQUENT_ESP32CAMERA_CAMERA_CAMERA
#define ELOQUENT_ESP32CAMERA_CAMERA_CAMERA

#include <esp_camera.h>
#include "./quality.h"
#include "./brownout.h"
#include "./xclk.h"
#include "./resolution.h"
#include "./pinout.h"
#include "./sensor.h"
#include "./pixformat.h"
#include "./rgb_565.h"
#include "../extra/exception.h"
#include "../extra/time/rate_limit.h"
#include "../extra/esp32/multiprocessing/mutex.h"

using Eloquent::Error::Exception;
using Eloquent::Extra::Time::RateLimit;
using Eloquent::Extra::Esp32::Multiprocessing::Mutex;

namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Configure and use the camera,
             * Eloquent style
             */
            class Camera {
                public:
                    camera_config_t config;
                    camera_fb_t *frame;
                    JpegQuality quality;
                    Brownout brownout;
                    XCLK xclk;
                    Resolution resolution;
                    Pinout pinout;
                    Sensor sensor;
                    Pixformat pixformat;
                    Exception exception;
                    RateLimit rateLimit;
                    Mutex mutex;
                    Converter565<Camera> rgb565;

                    /**
                     * Constructor
                     */
                    Camera() :
                        exception("Camera"),
                        mutex("Camera"),
                        rgb565(this) {

                    }

                    /**
                     *
                     * @return
                     */
                    Exception& begin() {
                        // assert pinout is set
                        if (!pinout)
                            return exception.set("Pinout not set");

                        // assign pins
                        config.pin_d0 = pinout.pins.d0;
                        config.pin_d1 = pinout.pins.d1;
                        config.pin_d2 = pinout.pins.d2;
                        config.pin_d3 = pinout.pins.d3;
                        config.pin_d4 = pinout.pins.d4;
                        config.pin_d5 = pinout.pins.d5;
                        config.pin_d6 = pinout.pins.d6;
                        config.pin_d7 = pinout.pins.d7;
                        config.pin_xclk = pinout.pins.xclk;
                        config.pin_pclk = pinout.pins.pclk;
                        config.pin_vsync = pinout.pins.vsync;
                        config.pin_href = pinout.pins.href;
                        config.pin_sccb_sda = pinout.pins.sccb_sda;
                        config.pin_sccb_scl = pinout.pins.sccb_scl;
                        config.pin_pwdn = pinout.pins.pwdn;
                        config.pin_reset = pinout.pins.reset;

                        config.ledc_channel = LEDC_CHANNEL_0;
                        config.ledc_timer = LEDC_TIMER_0;
                        config.fb_count = 1;
                        config.pixel_format = pixformat.format;
                        config.frame_size = resolution.framesize;
                        config.jpeg_quality = quality.quality;
                        config.xclk_freq_hz = xclk.freq;
                        config.grab_mode = CAMERA_GRAB_LATEST;

                        // init
                        if (esp_camera_init(&config) != ESP_OK)
                            return exception.set("Cannot init camera");

                        sensor.setFrameSize(resolution.framesize);

                        return exception.clear();
                    }

                    /**
                     * Manually set frame
                     *
                     * @param newFrame
                     */
                    void setFrame(camera_fb_t *newFrame) {
                        frame = newFrame;
                    }

                    /**
                     * Capture new frame
                     */
                    Exception& capture() {
                        if (!rateLimit)
                            return exception.soft().set("Too many requests for frame");

                        mutex.threadsafe([this]() {
                            free();
                            frame = esp_camera_fb_get();
                        }, 1000);

                        if (!mutex.isOk())
                            return exception.set("Cannot acquire mutex");

                        rateLimit.touch();

                        if (!hasFrame())
                            return exception.set("Cannot capture frame");

                        return exception.clear();
                    }

                    /**
                     * Release frame memory.
                     * Only call if you know what you're doing:
                     * calling this method at the wrong time may
                     * 1) make other pieces of code of this library break
                     * 2) introduce lags in the capture process
                     *
                     * Frame buffer is already release before each new capture,
                     * so you shouldn't have the necessity to call this method
                     * on your own.
                     */
                    void free() {
                        if (frame != NULL) {
                            esp_camera_fb_return(frame);
                            frame = NULL;
                        }
                    }

                    /**
                     * Test if camera has a valid frame
                     */
                    inline bool hasFrame() const {
                        return frame != NULL && frame->len > 0;
                    }

                    /**
                     * Get frame size in bytes
                     * @return
                     */
                    inline size_t getSizeInBytes() {
                        return hasFrame() ? frame->len : 0;
                    }

                    /**
                     * Save to given folder with automatic name
                     */
                    template<typename Disk>
                    Exception& saveTo(Disk& disk, String folder = "") {
                        return saveToAs(disk, folder, "");
                    }

                    /**
                     * Save to given folder with given name
                     */
                    template<typename Disk>
                    Exception& saveToAs(Disk& disk, String folder = "", String filename = "") {
                        if (!hasFrame())
                            return exception.set("No frame to save");

                        if (filename == "")
                            filename = disk.getNextFilename("jpg");

                        if (folder != "")
                            filename = folder + '/' + filename;

                        if (filename[0] != '/')
                            filename = String('/') + filename;
                        
                        return disk.writeBinary(filename, frame->buf, frame->len);
                    }

                    /**
                     * Sometimes you may need to swap RGB565 bytes
                     */
                    void swapBytes() {
                        if (!hasFrame())
                            return;

                        for (size_t i = 0; i < frame->len; i += 2) {
                            const uint8_t tmp = frame->buf[i];
                            frame->buf[i] = frame->buf[i + 1];
                            frame->buf[i + 1] = tmp;
                        }
                    }

                protected:
            };
        }
    }
}

namespace eloq {
    static Eloquent::Esp32cam::Camera::Camera camera;
}

#endif