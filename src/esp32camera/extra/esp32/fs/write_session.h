#ifndef ELOQUENT_EXTRA_ESP32_FS_WRITE_SESSION
#define ELOQUENT_EXTRA_ESP32_FS_WRITE_SESSION

#include <FS.h>
#include "../../exception.h"
#include "../nvs/counter.h"

using Eloquent::Extra::Exception;
using Eloquent::Extra::Esp32::NVS::Counter;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Handle file writing
                 */
                class WriteSession {
                    public:
                        char *mode;
                        uint8_t *data;
                        size_t length;
                        String folder;
                        String lastFilename;
                        String lastPath;
                        Exception exception;
                        fs::FS *fsystem;

                        /**
                         * 
                         */
                        WriteSession(fs::FS *filesystem) :
                            fsystem(filesystem),
                            mode("wb"),
                            data(NULL),
                            length(0),
                            folder(""),
                            lastFilename(""),
                            lastPath(""),
                            counter("cam_frames"),
                            exception("WriteSession") {

                            }

                        /**
                         * Set data
                        */
                        void open(uint8_t *data, size_t length) {
                            this->data = data;
                            this->length = length;
                            this->mode = (char*) "wb";
                        }

                        /**
                         * Set folder
                         */
                        WriteSession& inside(String folder_) {
                            folder = folder_;

                            if (folder == "")
                                return *this;

                            folder = toAbs(untrail(folder));

                            const char *fname = folder.c_str();
                            ESP_LOGE("WriteSession", "Creating folder %s", fname);
                            fsystem->mkdir(folder);

                            return *this;
                        }

                        /**
                         * 
                         */
                        Exception& to(String filename, String ext = "") {
                            if (data == NULL || length == 0)
                                return exception.set("Cannot write empty data");

                            if (filename == "")
                                filename = counter.nextString();

                            if (ext != "")
                                filename += String(".") + ext;

                            lastFilename = filename;
                            
                            if (folder != "") {
                                filename = folder + '/' + filename;
                            }

                            folder = "";
                            filename = toAbs(filename);
                            filename.replace("//", "/");

                            // write binary
                            if (mode == "wb") {
                                const char *fname = filename.c_str();
                                ESP_LOGI("WriteSession", "Writing %d bytes to file %s", length, fname);
                            
                                auto file = fsystem->open(filename, mode);

                                if (!file)
                                    return exception.set(String("Cannot open file ") + filename);

                                size_t written = file.write(data, length);
                                file.close();

                                if (written != length)
                                    return exception.set(String("Write length mismatch: written ") + written + " vs " + length + " expected");
                            }

                            return exception.clear();
                        }

                    protected:
                        Counter counter;

                        /**
                         * 
                         */
                        String toAbs(String path) {
                            return path.startsWith("/") ? path : String('/') + path;
                        }

                        /**
                         * 
                         */
                        String untrail(String path) {
                            return path.endsWith("/") ? path.substring(0, path.length() - 1) : path;
                        }
                };
            }
        }
    }
}

#endif