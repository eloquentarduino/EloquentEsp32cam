#ifndef ELOQUENT_ESP32CAM_CAMERA_SPIFFS
#define ELOQUENT_ESP32CAM_CAMERA_SPIFFS

#include <FS.h>
#include <SPIFFS.h>
#include "../../exception.h"
#include "./write_session.h"


using Eloquent::Extra::Exception;
using namespace eloq;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Interact with SPIFFS
                 */
                class Spiffs {
                public:
                    Exception exception;
                    WriteSession session;

                    /**
                     * 
                     */
                    Spiffs() : 
                        exception("SPIFFS"),
                        root("/spiffs"),
                        partitionLabel(""),
                        format(false),
                        maxFiles(5),
                        session(&SPIFFS) {
                    }

                    /**
                     * 
                     */
                    fs::FS& fs() {
                        return SPIFFS;
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
                    bool format;
                    String root;
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