#ifndef ELOQUENT_ESP32CAM_CAMERA_SDMMC
#define ELOQUENT_ESP32CAM_CAMERA_SDMMC

#include <FS.h>
#include <SD_MMC.h>
#include "../../error/Exception.h"
#include "../nvs/counter.h"
#include "../ntp.h"
#include "./sdmmc_pins.h"


using Eloquent::Extra::Error::Exception;
using Eloquent::Extra::Esp32::NVS::Counter;

using namespace e;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Store pictures on the SD
                 */
                class SDMMC {
                public:
                    Exception exception;
                    Pinout pinout;

                    /**
                     * 
                     */
                    SDMMC() : 
                        exception("SDMMC"),
                        root("/sdcard"), 
                        freq(SDMMC_FREQ_DEFAULT), 
                        maxFiles(5) {
                    }

                    /**
                     * 
                    */
                    void mountAt(String mountpoint) {
                        root = mountpoint;
                    }

                    /**
                     * Set high read/write speed
                    */
                    void highSpeed() {
                        speed(SDMMC_FREQ_HIGHSPEED);
                    }

                    /**
                     * Set custom speed
                     */
                    void speed(int speed) {
                        freq = speed;
                    }

                    /**
                     * Set max number of open files
                     */
                    void maxOpenFiles(uint8_t numMaxFiles) {
                        maxFiles = numMaxFiles;
                    }

                    /**
                     * Init SD
                     */
                    Exception& begin() {
                        bool isOneBit = (pinout.pins.d1 == 0 && pinout.pins.d2 == 0);
                        bool formatOnFailure = true;

                        if (pinout.pins.clk != pinout.pins.cmd)
                            SD_MMC.setPins(pinout.pins.clk, pinout.pins.cmd, pinout.pins.d0);

                        if (!isOneBit)
                            SD_MMC.setPins(pinout.pins.clk, pinout.pins.cmd, pinout.pins.d0, pinout.pins.d1, pinout.pins.d2, pinout.pins.d3);

                        if (!SD_MMC.begin(root.c_str(), isOneBit, formatOnFailure, freq, maxFiles))
                            return exception.set("Something went wrong with SD_MMC.begin()");

                        if (SD_MMC.cardType() == CARD_NONE)
                            return exception.set("Cannot detect any SD card");

                        return exception.clear();
                    }

#ifdef ELOQUENT_ESP32CAMERA_CAMERA_CAMERA
                    /**
                     * Save current frame to file
                     * with automatic naming scheme
                     */
                    Exception& saveCurrentFrame() {
                        Counter counter("cam_frames");
                        String filename = counter.nextString();

                        // append timestamp to filename
                        ntp.refresh();

                        if (!!ntp) {
                            filename += "_";
                            filename += ntp.filenameDatetime();
                        }

                        return saveCurrentFrameAs(filename);
                    }

                    /**
                     * Save current frame to file
                     */
                    Exception& saveCurrentFrameAs(String filename) {
                        if (!camera.hasFrame())
                            return exception.set("Cannot save empty frame");

                        if (!filename.endsWith(".jpg") && !filename.endsWith(".jpeg"))
                            filename = filename + ".jpg";

                        return saveAs(filename, camera.frame->buf, camera.frame->len);
                    }
#endif

                    /**
                     * Save binary data to file
                     */
                    Exception& saveAs(String filename, uint8_t *buf, size_t length) {
                        if (buf == NULL || length == 0)
                            return exception.set("Cannot save empty data");

                        const char *filename_str = filename.c_str();
                        ESP_LOGI("SDMMC", "Writing %d bytes to file %s", length, filename_str);
                        
                        auto file = SD_MMC.open(filename, "wb");

                        if (!file)
                            return exception.set(String("Cannot open file ") + filename);

                        size_t written = file.write(buf, length);
                        file.close();

                        if (written != length)
                            return exception.set(String("Write length mismatch: written ") + written + " vs " + length + " expected");

                        return exception.clear();
                    }

                protected:
                    String root;
                    int freq;
                    uint8_t maxFiles;
                };
            }
        }
    }
}

namespace e {
    static Eloquent::Extra::Esp32::Fs::SDMMC sdmmc;
}

#endif