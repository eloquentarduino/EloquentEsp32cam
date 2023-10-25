#ifndef ELOQUENT_ESP32CAM_VIZ_IMAGE_BROWSER
#define ELOQUENT_ESP32CAM_VIZ_IMAGE_BROWSER

#include <FS.h>
#include "../extra/exception.h"
#include "../extra/esp32/wifi/sta.h"
#include "../extra/esp32/http/server.h"

using namespace eloq;
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
                            WiFiClient client = web->client();
                            const uint8_t index[] = {31, 139, 8, 0, 0, 0, 0, 0, 0, 19, 181, 86, 91, 111, 219, 54, 20, 126, 247, 175, 96, 133, 96, 150, 81, 91, 114, 87, 96, 5, 12, 203, 43, 134, 161, 192, 208, 53, 15, 29, 176, 61, 116, 5, 66, 75, 84, 204, 134, 34, 5, 146, 174, 157, 5, 250, 239, 61, 188, 200, 177, 108, 93, 156, 180, 227, 131, 46, 135, 223, 249, 116, 174, 60, 90, 102, 244, 43, 218, 207, 82, 38, 240, 29, 220, 51, 172, 113, 18, 252, 81, 224, 91, 242, 155, 20, 59, 69, 100, 56, 9, 86, 35, 4, 107, 105, 144, 41, 195, 74, 37, 65, 78, 25, 81, 168, 192, 251, 217, 110, 246, 122, 207, 60, 194, 162, 52, 94, 51, 82, 227, 118, 179, 124, 203, 142, 183, 29, 100, 67, 112, 214, 148, 57, 185, 60, 23, 122, 133, 213, 181, 88, 198, 112, 235, 220, 127, 7, 22, 113, 92, 144, 97, 148, 162, 255, 13, 160, 126, 252, 46, 72, 79, 124, 51, 184, 179, 40, 44, 245, 90, 100, 247, 109, 145, 33, 69, 201, 176, 38, 144, 33, 154, 39, 193, 11, 27, 255, 8, 60, 190, 213, 155, 160, 203, 152, 142, 104, 186, 205, 12, 165, 130, 169, 18, 243, 36, 248, 165, 131, 161, 94, 215, 2, 185, 124, 231, 98, 203, 179, 110, 206, 88, 183, 36, 213, 239, 180, 216, 2, 82, 239, 85, 191, 195, 185, 144, 174, 224, 166, 136, 34, 202, 157, 45, 207, 117, 122, 63, 211, 100, 175, 147, 128, 162, 151, 232, 85, 176, 234, 182, 249, 84, 193, 124, 53, 50, 5, 246, 100, 165, 82, 18, 173, 239, 255, 130, 170, 187, 64, 181, 63, 19, 127, 83, 178, 123, 70, 252, 47, 226, 254, 104, 219, 231, 255, 98, 255, 93, 236, 56, 28, 49, 207, 169, 158, 203, 248, 9, 35, 250, 57, 214, 63, 173, 54, 65, 126, 222, 161, 75, 157, 11, 161, 159, 116, 158, 29, 117, 223, 155, 158, 238, 59, 237, 251, 13, 86, 215, 80, 88, 3, 253, 186, 92, 111, 181, 22, 28, 189, 77, 25, 77, 239, 76, 253, 125, 37, 28, 170, 209, 196, 255, 131, 144, 80, 134, 127, 194, 19, 42, 224, 113, 25, 59, 108, 143, 9, 61, 93, 234, 247, 219, 142, 242, 182, 243, 174, 25, 37, 16, 152, 73, 225, 103, 75, 12, 195, 101, 53, 170, 111, 42, 149, 180, 212, 72, 201, 20, 124, 214, 186, 84, 139, 56, 78, 51, 30, 105, 76, 217, 142, 242, 44, 85, 42, 74, 69, 97, 250, 201, 65, 79, 116, 226, 120, 203, 203, 187, 91, 131, 137, 49, 43, 41, 39, 95, 84, 128, 50, 146, 19, 121, 174, 226, 44, 48, 180, 98, 23, 29, 79, 62, 148, 160, 124, 203, 83, 77, 5, 15, 39, 232, 225, 96, 57, 244, 243, 86, 242, 35, 129, 89, 34, 207, 21, 209, 11, 52, 159, 54, 196, 140, 22, 20, 164, 175, 79, 196, 246, 12, 91, 160, 79, 159, 155, 98, 34, 165, 144, 45, 114, 159, 247, 5, 202, 49, 83, 100, 58, 106, 108, 82, 78, 117, 195, 190, 122, 233, 13, 85, 81, 78, 116, 186, 177, 99, 47, 156, 180, 35, 174, 118, 24, 32, 225, 216, 154, 58, 158, 34, 224, 74, 86, 45, 116, 7, 5, 231, 42, 68, 103, 222, 141, 233, 253, 108, 213, 20, 85, 39, 14, 213, 117, 218, 237, 148, 183, 224, 101, 226, 94, 173, 229, 189, 148, 199, 230, 180, 144, 218, 237, 240, 198, 148, 26, 84, 218, 213, 131, 47, 6, 38, 82, 108, 178, 31, 109, 132, 210, 85, 108, 147, 246, 171, 251, 118, 114, 245, 112, 100, 73, 245, 147, 53, 161, 22, 218, 151, 234, 230, 220, 113, 179, 34, 152, 251, 60, 148, 48, 78, 33, 202, 112, 139, 204, 168, 8, 39, 151, 130, 37, 45, 194, 73, 164, 74, 6, 73, 15, 254, 229, 65, 175, 34, 131, 202, 87, 221, 217, 52, 43, 21, 92, 105, 63, 223, 19, 100, 21, 122, 15, 151, 8, 160, 26, 254, 10, 13, 210, 48, 155, 123, 164, 52, 150, 90, 253, 67, 53, 148, 145, 161, 90, 140, 59, 204, 58, 176, 20, 184, 108, 82, 72, 2, 135, 76, 74, 106, 253, 41, 26, 63, 157, 195, 69, 101, 60, 189, 72, 51, 252, 100, 230, 221, 20, 153, 223, 193, 207, 182, 228, 195, 238, 40, 213, 203, 170, 12, 162, 12, 229, 48, 234, 241, 191, 96, 225, 123, 70, 200, 2, 235, 250, 23, 53, 52, 151, 126, 63, 42, 240, 115, 32, 177, 238, 68, 249, 49, 153, 181, 92, 223, 147, 90, 79, 224, 115, 59, 96, 185, 63, 243, 106, 211, 219, 172, 67, 73, 146, 160, 241, 130, 155, 163, 113, 60, 241, 191, 195, 104, 5, 231, 82, 39, 183, 11, 180, 175, 246, 199, 23, 152, 20, 28, 122, 61, 180, 47, 221, 254, 89, 133, 67, 72, 221, 67, 63, 248, 209, 11, 255, 212, 10, 31, 56, 16, 219, 10, 163, 165, 165, 105, 142, 236, 30, 90, 162, 87, 243, 249, 188, 221, 13, 63, 188, 110, 174, 30, 12, 180, 66, 235, 123, 77, 212, 205, 1, 58, 68, 122, 1, 175, 131, 199, 206, 134, 72, 139, 119, 116, 79, 178, 240, 231, 73, 133, 222, 175, 123, 62, 212, 78, 112, 198, 241, 225, 136, 195, 198, 106, 212, 124, 170, 70, 135, 1, 255, 13, 67, 108, 231, 90, 219, 14, 0, 0};

                            client.println(F("HTTP/1.1 200 OK"));
                            client.println(F("Content-Type: text/html"));
                            client.print(F("Content-Length: "));
                            client.println(sizeof(index));
                            client.println(F("Content-Encoding: gzip\r\n"));
                            client.write(index, sizeof(index));
                        });
                    }

                    /**
                     * Register /files endpoint to get list of files
                     */
                    void onFiles() {
                        server.onGET("/files", [this](WebServer *web) {
                            const uint8_t limit = 40;
                            const uint16_t offset = 0;
                            uint16_t i = 0;
                            bool hasNextPage = false;
                            File root = _fs->open("/");
                            File file = root.openNextFile();    

                            // empty list
                            if (!file) {
                                root.close();
                                web->send(500, "text/plain", F("No files found"));
                                return;
                            }

                            // list files
                            while (file) {
                                String filename = file.name();

                                if (file.isDirectory()) {
                                    web->sendContent("folder:");
                                    web->sendContent(filename);
                                    web->sendContent(",");
                                    web->sendContent(String(file.size()));
                                    web->sendContent("\n");

                                    file = root.openNextFile();
                                    continue;
                                }

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
                                if (i >= offset + limit) {
                                    hasNextPage = true;
                                    break;
                                }

                                web->sendContent("file:");
                                web->sendContent(filename);
                                web->sendContent(",");
                                web->sendContent(String(file.size()));
                                web->sendContent("\n");

                                file = root.openNextFile();
                            }

                            if (hasNextPage)
                                web->sendContent(":next:");

                            web->sendContent("");
                            root.close();
                        });
                    }
            };
        }
    }
}

namespace eloq {
    static Eloquent::Esp32cam::Viz::ImageBrowser browser;
}

#endif