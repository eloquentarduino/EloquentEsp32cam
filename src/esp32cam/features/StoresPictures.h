//
// Created by Simone on 23/04/23.
//

#ifndef ELOQUENTESP32CAM_STORESPICTURES_H
#define ELOQUENTESP32CAM_STORESPICTURES_H


#include <FS.h>
#include <Preferences.h>
#include "../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Syntactic sugar to save pictures to different
             * filesystems with incrementing filename
             */
             template<typename Camera>
            class StoresPictures : public HasErrorMessage {
            public:
                String lastFilename;

                /**
                 *
                 */
                StoresPictures(Camera* camera) :
                    cam(camera),
                    driver('0'),
                    ns("") {

                }

                /**
                 * Initialize SPIFFS
                 * @return
                 */
#ifdef _SPIFFS_H_
                bool spiffs() {
                    if (!SPIFFS.begin(true))
                        return setErrorMessage("SPIFFS.begin(true) failed");

                    driver = 'S';
                    ESP_LOGD("Storage", "SPIFFS init OK");

                    return setErrorMessage("");
                }
#endif

                /**
                 * Initialize SD MMC
                 * @return
                 */
#ifdef _SDMMC_H_
                bool sdmmc() {
                    if (!SD_MMC.begin())
                        return setErrorMessage("SD_MMC.begin() failed");

                    if (SD_MMC.cardType() == CARD_NONE)
                        return setErrorMessage("No SD Card detected");

                    driver = 'M';
                    ESP_LOGD("Storage", "SD_MMC init OK");

                    return setErrorMessage("");
                }
#endif

                /**
                 * Set namespace
                 */
                 void setNamespace(String namesp) {
                     ns = namesp;
                 }

                 /**
                  * Reset incrementing counter
                  */
                 void resetCounter() {
                     Preferences prefs;

                     prefs.begin("storage", false);
                     prefs.putULong("uuid", 0);
                     prefs.end();
                 }

                /**
                 * Get next filename (always incrementing)
                 * @return
                 */
                String getNextFilename() {
                    Preferences prefs;
                    unsigned long uuid;
                    char counter[15];
                    String datetime;

                    // use incremental counter
                    prefs.begin("storage", false);
                    uuid = prefs.getULong("uuid", 0);
                    prefs.putULong("uuid", uuid + 1);
                    prefs.end();
                    sprintf(counter, "%010lu", uuid);

                    // append timestamp, if available
                    if (cam->ntp.refresh()) {
                        datetime = String("_") + cam->ntp.getDateTime();
                    }

                    return String("/") + (ns != "" ? ns + "/" : "") + counter + datetime + ".jpg";
                }

                /**
                 * Get current filesystem
                 * @return
                 */
                fs::FS* getFS() {
#ifdef _SPIFFS_H_
                    if (driver == 'S')
                        return &SPIFFS;
#endif
#ifdef _SDMMC_H_
                    if (driver == 'M')
                        return &SD_MMC;
#endif

                    return NULL;
                }

                /**
                 * Save picture to filesystem
                 */
                bool save(String filename = "") {
                    fs::FS *fs = getFS();

                    if (fs == NULL)
                        return setErrorMessage("Filesystem not set");

                    if (filename == "")
                        filename = getNextFilename();

                    ESP_LOGI("Storage", "Storing picture to %s://%s", getDriverName(), filename.c_str());

                    lastFilename = filename;

                    auto file = fs->open(filename, "wb");
                    unsigned int bytes = file.write(cam->frame->buf, cam->frame->len);
                    file.close();

                    ESP_LOGD("Storage", "Written %u bytes", bytes);

                    return setErrorMessage("");
                }

            protected:
                char driver;
                String ns;
                Camera *cam;

                /**
                 *
                 * @return
                 */
                const char* getDriverName() {
                    switch (driver) {
                        case 'S':
                            return "SPIFFS";
                        case 'M':
                            return "SD_MMC";
                        case 'D':
                            return "SD";
                        default:
                            return "???";
                    }
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_STORESPICTURES_H
