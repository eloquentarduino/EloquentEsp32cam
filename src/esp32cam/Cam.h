//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_CAM_H
#define ELOQUENTESP32CAM_CAM_H

#include <esp_camera.h>
#include <FS.h>
#include "../traits/SetFrameSize.h"
#include "../traits/SetJpegQuality.h"
#include "../traits/SetModelPins.h"
#include "../traits/ConfiguresImageSensor.h"
#include "../traits/Debounces.h"
#include "../traits/ConnectsToWiFi.h"
#include "../traits/DecodesJpeg.h"
#include "../traits/HasMDNS.h"
#include "../traits/SavesToFilesystem.h"


namespace Eloquent {
    namespace Esp32cam {
        
        /**
         * Esp32 cam interface
         */
        class Cam :
                public SetFrameSize,
                public SetJpegQuality,
                public SetModelPins,
                public ConfiguresImageSensor,
                public Debounces,
                public DecodesJpeg,
                public HasMDNS,
                public SavesToFilesystem,
                public ConnectsToWiFi {
            
        public:
            camera_config_t config;
            camera_fb_t *frame;
            
            /**
             *
             */
            Cam() {
                highQuality();
                vga();
                debounce(0);
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

                setJpeg(frame->buf, frame->len);

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

        protected:
        };
    }
}


#endif //ELOQUENTESP32CAM_CAM_H
