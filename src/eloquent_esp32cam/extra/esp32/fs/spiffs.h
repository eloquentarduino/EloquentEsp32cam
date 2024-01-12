#ifndef ELOQUENT_ESP32CAM_CAMERA_SPIFFS
#define ELOQUENT_ESP32CAM_CAMERA_SPIFFS

#include <FS.h>
#include <SPIFFS.h>
#include "./fs.h"


using namespace eloq;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Interact with SPIFFS
                 */
                class Spiffs : public FileSystem {
                public:
                    /**
                     * 
                     */
                    Spiffs() : 
                        FileSystem("SPIFFS", &SPIFFS, "/spiffs"),
                        partitionLabel(""),
                        format(false),
                        maxFiles(5) {
                    }

                    /**
                     * 
                     */
                    void formatOnFail() {
                        format = true;
                    }

                    /**
                     * 
                     */
                    void mountAt(String mountpoint) {
                        root = mountpoint;
                    }

                    /**
                     * Set partition label
                     */
                    void partition(String partition) {
                        partitionLabel = partition;
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
                        const char *part = partitionLabel != "" ? partitionLabel.c_str() : NULL;

                        if (!SPIFFS.begin(format, root.c_str(), maxFiles, part))
                            return exception.set("Something went wrong with SPIFFS.begin()");

                        return exception.clear();
                    }

                    

                protected:
                    bool format;
                    String partitionLabel;
                    uint8_t maxFiles;
                };
            }
        }
    }
}

namespace eloq {
    static Eloquent::Extra::Esp32::Fs::Spiffs spiffs;
}

#endif