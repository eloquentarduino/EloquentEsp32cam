#ifndef ELOQUENT_ESP32CAM_BROWSER_IMAGEBROWSERSERVER
#define ELOQUENT_ESP32CAM_BROWSER_IMAGEBROWSERSERVER

#ifndef IMAGEBROWSER_SERVER_STACK_SIZE
#define IMAGEBROWSER_SERVER_STACK_SIZE 5000
#endif

//#include "../assets/ImageBrowserServer.js.h"
#include "../assets/ImageBrowserServer.alpine.h"
#include "../camera/Camera.h"
#include "../extra/error/Exception.h"
#include "../extra/esp32/fs/Filesystem.h"
#include "../extra/esp32/wifi/WiFiSta.h"
#include "../extra/esp32/http/HttpServerThread.h"
#include "../extra/esp32/html/HtmlBuilder.h"

using namespace e;
using Eloquent::Extra::Error::Exception;
using Eloquent::Extra::Esp32::Fs::Filesystem;
using Eloquent::Extra::Esp32::Http::HttpServerThread;
using Eloquent::Extra::Esp32::Html::HtmlBuilder;

namespace Eloquent {
    namespace Esp32cam {
        namespace Browser {
            class ImageBrowserServer {
                public:
                    Exception exception;
                    fs::FS *fs;
                    HttpServerThread httpServerThread;
                    HtmlBuilder html;

                    /**
                     * Constructor
                     */
                    ImageBrowserServer(uint8_t port = 80) :
                        exception("ImageBrowserServer"),
                        httpServerThread("ImageBrowserServer", port),
                        html(httpServerThread.httpServer.webServer),
                        fs(ELOQUENT_CURRENT_FILESYSTEM.fs.fs),
                        numFilesPerPage(30) {

                        }

                    /**
                     * Set default number of files to show for each page
                     */
                    void setNumFilesPerPage(uint8_t filesPerPage) {
                        numFilesPerPage = filesPerPage;
                    }

                    /**
                     * Start server
                     */
                    Exception& begin(uint16_t port = 80) {
                        if (!wifiSta.isConnected())
                            return exception.set("WiFi not connected");

                        httpServerThread.httpServer.setPort(port);

                        // API
                        httpServerThread.httpServer.on("/api/list", [this]() {
                            list(); 
                        });
                        // httpServerThread.httpServer.on("/view", [this]() { view(); });
                        // httpServerThread.httpServer.on("/delete", [this]() { remove(); });
                        // httpServerThread.httpServer.on("/rename", [this]() { rename(); });
                        httpServerThread.httpServer.onPOST("/capture", [this]() { capture(); });

                        // run in thread
                        httpServerThread.thread.withStackSize(IMAGEBROWSER_SERVER_STACK_SIZE);

                        if (!httpServerThread.begin().isOk())
                            return httpServerThread.exception;

                        return exception.clear();
                    }

                protected:
                    uint8_t numFilesPerPage;

                    /**
                     * 
                     */
                    void list() {
                        if (ELOQUENT_CURRENT_FILESYSTEM.exception)
                            httpServerThread.httpServer.abortByServer(ELOQUENT_CURRENT_FILESYSTEM.exception);

                        File root = fs->open("/");
                        File file = root.openNextFile();

                        // empty list
                        if (!file)
                            httpServerThread.httpServer.webServer.send(200, "text/plain", F("{\"error\": \"No file found\"}\n"));

                        httpServerThread.httpServer.textStream([this, &root, &file](WebServer& webServer) {
                            const uint16_t page = httpServerThread.httpServer.getIntArg("page", 0);
                            const uint16_t perPage = httpServerThread.httpServer.getIntArg("perPage", numFilesPerPage);
                            const uint16_t offset = page * perPage;
                            uint16_t i = 0;
                            bool hasNextPage = false;

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

                                // stream one by one as JSON-line
                                char fileAsJsonLine[200] = {'\0'};
                                sprintf(fileAsJsonLine, "{\"filename\": \"%s\", \"size\": %lu}\n", filename.c_str(), (unsigned long) file.size());
                                httpServerThread.httpServer.webServer.sendContent(fileAsJsonLine);

                                file = root.openNextFile();
                            }

                            if (hasNextPage)
                                httpServerThread.httpServer.webServer.sendContent(F("{\"hasNextPage\":true}\n"));
                        });
                    }

                    /**
                 * Capture new photo
                 */
                void capture() {
                    if (!camera.capture().isOk()) {
                        httpServerThread.httpServer.abortByServer(camera.exception);
                        return;
                    }

                    if (!camera.saveTo(ELOQUENT_CURRENT_FILESYSTEM).isOk()) {
                        httpServerThread.httpServer.abortByServer(camera.exception);
                        return;
                    }

                    String filename = ELOQUENT_CURRENT_FILESYSTEM.lastFilename;
                    size_t filesize = ELOQUENT_CURRENT_FILESYSTEM.lastFilesize;
                    String response = 
                        String("{\"status\":\"OK\", \"filename\":\"")
                        + filename
                        + "\", \"size\":"
                        + filesize
                        + '}';

                    httpServerThread.httpServer.ok(response);
                }
            };
        }
    }
}

namespace e {
    static Eloquent::Esp32cam::Browser::ImageBrowserServer imageBrowserServer;
}

#endif