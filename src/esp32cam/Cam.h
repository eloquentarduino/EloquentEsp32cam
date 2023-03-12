//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_CAM_H
#define ELOQUENTESP32CAM_CAM_H

#include <esp_camera.h>
#include <FS.h>
#include "../traits/HasErrorMessage.h"
#include "../traits/SetFrameSize.h"
#include "../traits/SetJpegQuality.h"
#include "../traits/SetModelPins.h"
#include "../traits/ConfiguresImageSensor.h"
#include "../traits/Debounces.h"
#include "../traits/ConnectsToWiFi.h"
#include "../traits/HasMDNS.h"
#include "../traits/SavesToFilesystem.h"
#include "./features/CloudStorageUploader.h"


namespace Eloquent {
    namespace Esp32cam {
        
        /**
         * Esp32 cam interface
         */
        class Cam :
                public HasErrorMessage,
                public SetFrameSize,
                public SetJpegQuality,
                public SetModelPins,
                public ConfiguresImageSensor,
                public Debounces,
                public HasMDNS,
                public SavesToFilesystem,
                public ConnectsToWiFi {
            
        public:
            camera_config_t config;
            camera_fb_t *frame;
            Features::CloudStorageUploader cloudStorageUploader;
            
            /**
             *
             */
            Cam() {
                highQuality();
                vga();
                debounce(0);
            }

            /**
             * Set clock speed to 1MHz
             */
            inline void slowClock() {
                config.xclk_freq_hz = 10000000;
            }

            /**
             * Set clock speed to 2MHz
             */
            inline void fastClock() {
                config.xclk_freq_hz = 20000000;
            }

            /**
             *
             * @return
             */
            bool begin() {
                if (!hasPinsSet())
                    return setErrorMessage("You must set a model");

                setPins(&config);
                config.ledc_channel = LEDC_CHANNEL_0;
                config.ledc_timer = LEDC_TIMER_0;
                config.fb_count = 1;
                config.pixel_format = PIXFORMAT_JPEG;
                config.frame_size = _framesize;
                config.jpeg_quality = _jpegQuality;

                if (!config.xclk_freq_hz)
                    config.xclk_freq_hz = 20000000;

                if (esp_camera_init(&config) != ESP_OK)
                    return setErrorMessage("Init error");

                sensor = esp_camera_sensor_get();
                sensor->set_framesize(sensor, _framesize);

                return true;
            }

            /**
             * Turn flashlight on
             */
            inline void flashlightOn() {
                if (_pins.flashlight >= 0)
                    digitalWrite(_pins.flashlight, HIGH);
            }

            /**
             * Turn flashlight off
             */
            inline void flashlightOff() {
                if (_pins.flashlight >= 0)
                    digitalWrite(_pins.flashlight, LOW);
            }

            /**
             * Test if frame is in memory
             * @return
             */
            inline bool captured() {
                return frame != NULL && frame->len > 0;
            }


            /**
             * Get Jpeg size
             * @return
             */
            inline uint16_t getFileSize() {
                return captured() ? frame->len : 0;
            }


            /**
             * Capture new frame
             */
            bool capture() {
                //if (!debounced())
                //    return setErrorMessage("Too many requests");

                free();
                frame = esp_camera_fb_get();

                if (!captured())
                    return setErrorMessage("Capture error");

                return touch();
            }

            /**
             * Release frame memory
             */
            void free() {
                if (frame != NULL) {
                    esp_camera_fb_return(frame);
                    frame = NULL;
                }
            }

            /**
             *
             * @param fs
             * @param filename
             * @return
             */
            bool saveTo(fs::FS &fs, String filename) {
                if (!this->captured())
                    return setErrorMessage("Save error: frame not found");

                return SavesToFilesystem::saveTo(fs, filename, frame->buf, frame->len);
            }

            /**
             * Upload current picture to https://esp32camstorage.eloquentarduino.com
             *
             * @param deviceToken
             * @return
             */
            bool uploadToCloudStorage(String deviceToken) {
                return setErrorMessage(cloudStorageUploader.upload(deviceToken, frame->buf, frame->len));
            }

            /**
             * Get HTTP address of camera
             * @param port
             * @return
             */
            String getAddress(uint16_t port = 80) {
                return
                String("http://")
                + this->getIP()
                + (port != 80 ? String(':') + port : "");
            }

        protected:
        };
    }
}


#endif //ELOQUENTESP32CAM_CAM_H
