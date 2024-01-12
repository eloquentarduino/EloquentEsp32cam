#ifndef ELOQUENT_EXTRA_ESP32_FS_H
#define ELOQUENT_EXTRA_ESP32_FS_H

#include "../../exception.h"
#include "./write_session.h"

using Eloquent::Error::Exception;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Abstract filesystem
                 */
                class FileSystem {
                    public:
                        Exception exception;
                        WriteSession session;
                        String root;

                        /**
                         * 
                         */
                        FileSystem(const char *name, fs::FS *concrete, String root_) :
                            exception(name),
                            session(concrete),
                            _fs(concrete),
                            root(root_) {

                            }

                        /**
                         * Get concrete filesystem implementation
                         */
                        fs::FS* fs() {
                            return _fs;
                        }

                        /**
                         * 
                         */
                        void mountAt(String mountpoint) {
                            root = mountpoint;
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
                            return save(content.c_str());
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

                        /**
                         * Iterate over files
                         */
                        template<typename Callback>
                        void forEach(Callback callback, size_t offset = 0, size_t limit = 0) {
                            size_t i = 0;
                            File root = _fs->open("/");

                            if (!root) {
                                ESP_LOGE("FS", "Cannot open root");
                                return;
                            }

                            if (!root.isDirectory()) {
                                ESP_LOGE("FS", "Root is not a directory");
                                return;
                            }

                            root.rewindDirectory();
                            File file = root.openNextFile();

                            if (!file)
                                ESP_LOGW("FS", "No file found");

                            while (file) {
                                ESP_LOGI("FS", "%d: %s", i, file.name());

                                // seek to required position
                                if (i++ < offset) {
                                    file = root.openNextFile();
                                    continue;
                                }

                                // break after reaching page limit
                                if (i >= offset + limit)
                                    break;

                                callback(file.name(), file.size(), file.isDirectory());
                                file = root.openNextFile();
                            }

                            root.close();
                        }

                    protected:
                        fs::FS *_fs;

                };
            }
        }
    }
}
#endif