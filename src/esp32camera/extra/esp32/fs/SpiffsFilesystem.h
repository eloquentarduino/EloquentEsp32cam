#ifndef ELOQUENT_EXTRA_ESP32_FS_SPIFFSFILESYSTEM
#define ELOQUENT_EXTRA_ESP32_FS_SPIFFSFILESYSTEM

#define ELOQUENT_CURRENT_FILESYSTEM spiffsFS

#include <SPIFFS.h>
#include "./Filesystem.h"
#include "../../error/Exception.h"

using Eloquent::Extra::Error::Exception;

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * SPIFFS-based filesystem
                 */
                class SpiffsFilesystem {
                    public:
                        Filesystem fs;
                        Exception exception;
                        String lastFilename;
                        size_t lastFilesize;

                        /**
                         * 
                         */
                        SpiffsFilesystem() :
                            fs(SPIFFS),
                            exception("SpiffsFS"),
                            lastFilename(""),
                            lastFilesize(0) {
                                exception.set("Not configured");
                            }

                        /**
                         * Init filesystem
                         */
                        Exception& begin() {
                            if (!SPIFFS.begin(true))
                                return exception.set("Cannot init SPIFFS");

                            return exception.clear();
                        }

                        /**
                         * Proxy
                         */
                        String getNextFilename(String ext) {
                            return fs.getNextFilename(ext);
                        }

                        /**
                         * Proxy
                         */
                        Exception& writeBinary(String filename, uint8_t *data, size_t length) {
                            if (fs.writeBinary(filename, data, length).isOk()) {
                                lastFilename = filename;
                                lastFilesize = length;
                            }
                            else {
                                lastFilename = "";
                                lastFilesize = 0;
                            }

                            return fs.exception;
                        }
                };
            }
        }
    }
}

namespace e {
    static Eloquent::Extra::Esp32::Fs::SpiffsFilesystem spiffsFS;
}

#endif