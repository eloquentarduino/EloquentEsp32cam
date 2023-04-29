//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_CAM_H
#define ELOQUENTESP32CAM_CAM_H

#include <esp_camera.h>
#include <FS.h>
#include "../traits/HasErrorMessage.h"
#include "../traits/Debounces.h"
#include "../traits/SavesToFilesystem.h"
#include "./features/ConfiguresPins.h"
#include "./features/SetsResolution.h"
#include "./features/ConfiguresImageSensor.h"
#include "./features/CloudStorageUploader.h"
#include "./features/StoresPictures.h"
#include "./features/ConnectsToWiFi.h"
#include "./features/SyncsTime.h"
#include "./features/SetsJpegQuality.h"
#include "./features/SetsClockFreq.h"



namespace Eloquent {
    namespace Esp32cam {
        
        /**
         * Esp32 cam interface
         */
        class Cam :
                public HasErrorMessage,
                public Debounces {
            
        public:
            camera_config_t config;
            camera_fb_t *frame;
            Features::ConfiguresPins<Cam> model;
            Features::SetsResolution resolution;
            Features::ConfiguresImageSensor sensor;
            Features::CloudStorageUploader cloudStorageUploader;
            Features::StoresPictures<Cam> storage;
            Features::ConnectsToWiFi wifi;
            Features::SyncsTime<Cam> ntp;
            Features::SetsJpegQuality quality;
            Features::SetsClockFreq xclk;
            
            /**
             *
             */
            Cam() :
                model(this),
                storage(this),
                ntp(this) {
                debounce(0);
            }

            /**
             *
             * @return
             */
            camera_config_t* getConfig() {
                return &config;
            }

            /**
             *
             * @return
             */
            bool begin() {
                config.ledc_channel = LEDC_CHANNEL_0;
                config.ledc_timer = LEDC_TIMER_0;
                config.fb_count = 1;
                config.pixel_format = PIXFORMAT_JPEG;
                config.frame_size = resolution.framesize;
                config.jpeg_quality = quality.quality;
                config.xclk_freq_hz = xclk.freq;

                if (!model.begin())
                    return setErrorMessage(model.getErrorMessage());

                sensor_t *sensor = esp_camera_sensor_get();
                sensor->set_framesize(sensor, resolution.framesize);

                return (wifi.autoconnect() && ntp.begin());
            }

            /**
             * Turn flashlight on
             */
            inline void flashlightOn() {
                //if (_pins.flashlight >= 0)
                //    digitalWrite(_pins.flashlight, HIGH);
            }

            /**
             * Turn flashlight off
             */
            inline void flashlightOff() {
                //if (_pins.flashlight >= 0)
                //    digitalWrite(_pins.flashlight, LOW);
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

                if (!captured()) {
                    return setErrorMessage("Cannot capture frame", "Camera");
                }

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
             * Upload current picture to https://esp32camstorage.eloquentarduino.com
             *
             * @param deviceToken
             * @return
             * @deprecated
             */
            bool uploadToCloudStorage(String deviceToken) {
                return setErrorMessage(cloudStorageUploader.upload(deviceToken, frame->buf, frame->len));
            }

        protected:
        };
    }
}


static Eloquent::Esp32cam::Cam camera;


#endif //ELOQUENTESP32CAM_CAM_H
