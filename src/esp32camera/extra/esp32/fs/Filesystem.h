#ifndef ELOQUENT_EXTRA_ESP32_FS_FILESYSTEM
#define ELOQUENT_EXTRA_ESP32_FS_FILESYSTEM

#include <FS.h>
#include <Preferences.h>
#include "../../error/Exception.h"
#include "../../ULID.h"

using Eloquent::Extra::Error::Exception;

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Interact with different filesystems
                 */
                class Filesystem {
                    public:
                        FS *fs;
                        Exception exception;

                        /**
                         * Constructor
                         */
                        Filesystem(FS& concreteFilesystem) :
                            fs(&concreteFilesystem),
                            exception("Filesystem") {

                            }
                        
                        /**
                         * Get filename for next file
                         */
                        String getNextFilename(String ext) {
                            Preferences prefs;
                            unsigned long seed;

                            prefs.begin(":ulid:", false);
                            seed = prefs.getULong("seed", 0) + 1;
                            prefs.putULong("seed", seed);
                            prefs.end();
                            
                            String ulid = ULID(seed, seed % 255).toString();

                            // @todo get time from NTP
                            
                            return ulid + '.' + ext;
                        }

                        /**
                         * Write binary file
                         */
                        Exception& writeBinary(String filename, uint8_t *data, size_t length) {
                            if (!length)
                                return exception.set("Cannot write empty content");

                            if (filename == "")
                                return exception.set("Cannot write to empty filename");

                            File file = fs->open(filename, "wb");
                            size_t written = file.write(data, length);
                            file.close();

                            if (written != length)
                                return exception.set("Written bytes != length");

                            ESP_LOGI("Filesystem", "Written %lu bytes to file %s", (unsigned long) written, filename.c_str());

                            return exception.clear();
                        }
                };
            }
        }
    }
}

#endif