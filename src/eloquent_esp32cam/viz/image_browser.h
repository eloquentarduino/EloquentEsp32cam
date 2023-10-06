#ifndef ELOQUENT_ESP32CAM_VIZ_IMAGE_BROWSER
#define ELOQUENT_ESP32CAM_VIZ_IMAGE_BROWSER

#include <FS.h>
#include "../extra/exception.h"
#include "../extra/esp32/wifi/sta.h"
#include "../extra/esp32/http/server.h"

using namespace e;
using Eloquent::Extra::Exception;
using Eloquent::Extra::Esp32::Http::HttpServer;

namespace Eloquent {
    namespace Esp32cam {
        namespace Viz {
            /**
             * CRUD pictures UI
             */
            class ImageBrowser {
                public:
                    Exception exception;
                    HttpServer server;

                    /**
                     * Constructor
                     */
                    ImageBrowser() :
                        exception("PictureBrowser"),
                        server("PictureBrowser"),
                        _fs(NULL) {

                        }

                    /**
                     * Set filesystem
                     */
                    void fs(fs::FS& fs) {
                        _fs = &fs;
                    }

                    /**
                     * Set filesystem
                     */
                    template<typename T>
                    void fs(T fs) {
                        _fs = &(fs.fs());
                    }

                    /**
                     * Dump self IP address
                     */
                    String address() const {
                        return String("PictureBrowser is available at http://") + wifi.ip();
                    }

                    /**
                     * Start server
                     */
                    Exception& begin() {
                        if (!wifi.isConnected())
                            return exception.set("WiFi not connected");

                        if (_fs == NULL)
                            return exception.set("No filesystem set");

                        onIndex();
                        onFiles();

                        // run in thread
                        server.thread.withStackSize(5000);

                        if (!server.begin().isOk())
                            return exception.propagate(server);

                        return exception.clear();
                    }

                protected:
                    fs::FS *_fs;

                    /**
                     * Register / endpoint to get index HTML
                     */
                    void onIndex() {
                        server.onGET("/", [this](WebServer *web) {
                        });
                    }

                    /**
                     * Register /files endpoint to get list of files
                     */
                    void onFiles() {
                        server.onGET("/files", [this](WebServer *web) {
                            const uint8_t perPage = 40;
                            const uint16_t offset = 0;
                            uint16_t i = 0;
                            bool hasNextPage = false;
                            File root = _fs->open("/");
                            File file = root.openNextFile();    

                            // empty list
                            if (!file) {
                                log_e("No files found");
                                web->send(200, "text/plain", F("!File not found"));
                                return;
                            }

                            // list files
                            while (file) {
                                String filename = file.name();

                                // only list jpeg files
                                if (filename.indexOf(".jpg") < 0 && filename.indexOf(".jpeg") < 0) {
                                    file = root.openNextFile();
                                    continue;
                                }

                                // seek to required position
                                if (i++ < offset) {
                                    file = root.openNextFile();
                                    continue;
                                }

                                // break after reaching page limit
                                if (i >= offset + perPage) {
                                    hasNextPage = true;
                                    break;
                                }

                                web->sendContent(filename);
                                web->sendContent(",");
                                web->sendContent(String(file.size()));
                                web->sendContent("\n");

                                file = root.openNextFile();
                            }

                            if (hasNextPage)
                                web->sendContent(":next");

                            web->sendContent("");
                        });
                    }
            };
        }
    }
}

namespace e {
    static Eloquent::Esp32cam::Viz::ImageBrowser browser;
}

#endif