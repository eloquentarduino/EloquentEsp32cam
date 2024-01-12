#ifndef ELOQUENT_EXTRA_ESP32_FS_WRITE_SESSION
#define ELOQUENT_EXTRA_ESP32_FS_WRITE_SESSION

#include <FS.h>
#include "../../exception.h"
#include "../nvs/counter.h"

using Eloquent::Error::Exception;
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
                        char *text;
                        size_t length;
                        String folder;
                        String lastFilename;
                        String lastPath;
                        Exception exception;
                        Counter counter;
                        fs::FS *fsystem;

                        /**
                         * 
                         */
                        WriteSession(fs::FS *filesystem) :
                            fsystem(filesystem),
                            mode("wb"),
                            data(NULL),
                            text(NULL),
                            length(0),
                            folder(""),
                            lastFilename(""),
                            lastPath(""),
                            counter("cam_frames"),
                            exception("WriteSession") {

                            }

                        /**
                         * Set binary data
                        */
                        void open(uint8_t *content, size_t size) {
                            data = content;
                            length = size;
                            text = NULL;
                            mode = (char*) "wb";
                        }

                        /**
                         * Set string data
                         */
                        void open(const char* content) {
                            data = NULL;
                            length = 0;
                            text = (char*) content;
                            mode = (char*) "w";
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
                            if ((data == NULL || length == 0) && (text == NULL))
                                return close("Cannot write empty data");

                            if (filename == "")
                                filename = counter.nextString();

                            if (ext != "")
                                filename += String(".") + ext;

                            ESP_LOGI("FS::Write", "Filename = %s", filename.c_str());

                            lastFilename = filename;
                            
                            if (folder != "") {
                                filename = folder + '/' + filename;
                            }

                            folder = "";
                            filename = toAbs(filename);
                            filename.replace("//", "/");

                            // write binary data
                            if (mode == "wb") {
                                const char *fname = filename.c_str();
                                ESP_LOGI("WriteSession", "Writing %d bytes to file %s", length, fname);
                            
                                auto file = fsystem->open(filename, "wb");

                                if (!file)
                                    return close(String("Cannot open file ") + filename);

                                size_t written = file.write(data, length);
                                file.close();

                                if (written != length)
                                    return close(String("Write length mismatch: written ") + written + " vs " + length + " expected");
                            }

                            // write string data
                            if (mode == "w") {
                                const char *fname = filename.c_str();
                                const uint16_t fsize = strlen(text);
                                ESP_LOGI("WriteSession", "Writing %d chars to file %s", fsize, fname);
                            
                                auto file = fsystem->open(filename, "w");

                                if (!file)
                                    return close(String("Cannot open file ") + filename);

                                file.print(text);
                                file.close();
                            }

                            return close("");
                        }

                    protected:

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

                        /**
                         * Close session
                         */
                        Exception& close(String error) {
                            data = NULL;
                            text = NULL;
                            length = 0;

                            return exception.set(error);
                        }
                };
            }
        }
    }
}

#endif