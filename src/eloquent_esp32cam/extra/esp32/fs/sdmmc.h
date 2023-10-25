#ifndef ELOQUENT_ESP32CAM_CAMERA_SDMMC
#define ELOQUENT_ESP32CAM_CAMERA_SDMMC

#include <FS.h>
#include <SD_MMC.h>
#include "../../exception.h"
#include "./sdmmc_pins.h"
#include "./write_session.h"


using Eloquent::Extra::Exception;
using namespace eloq;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Interact with the SD
                 */
                class SDMMC {
                public:
                    Exception exception;
                    Pinout pinout;
                    WriteSession session;

                    /**
                     * 
                     */
                    SDMMC() : 
                        exception("SDMMC"),
                        root("/sdcard"), 
                        freq(SDMMC_FREQ_DEFAULT), 
                        maxFiles(5),
                        session(&SD_MMC) {
                    }

                    /**
                     * 
                     */
                    fs::FS& fs() {
                        return SD_MMC;
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

                    /**
                     * Write string content
                     */
                    WriteSession& save(const char *content) {
                        session.open(content);

                        return session;
                    }

                    /**
                     * Write string content
                     */
                    WriteSession& save(String& content) {
                        session.open(content.c_str());

                        return session;
                    }

                    /**
                     * See save(data, length)
                     */
                    template<typename BinaryContent>
                    WriteSession& save(BinaryContent& content) {
                        return save(content->buf, content->len);
                    }

                    /**
                     * Write binary content
                     */
                    WriteSession& save(uint8_t *data, size_t length) {
                        session.open(data, length);

                        return session;
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

namespace eloq {
    static Eloquent::Extra::Esp32::Fs::SDMMC sdmmc;
}

#endif