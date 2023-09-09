#ifndef ELOQUENT_ESP32CAM_MJPEG_SERVER
#define ELOQUENT_ESP32CAM_MJPEG_SERVER

#ifndef MJPEG_SERVER_STACK_SIZE
#define MJPEG_SERVER_STACK_SIZE 5000
#endif

#include "../camera/Camera.h"
#include "../extra/error/Exception.h"
#include "../extra/esp32/wifi/WiFiSta.h"
#include "../extra/esp32/http/HttpServerThread.h"

using namespace e;
using Eloquent::Extra::Error::Exception;
using Eloquent::Extra::Esp32::Http::HttpServerThread;

namespace Eloquent {
    namespace Esp32cam {
        namespace Mjpeg {
            /**
             * Stream MJPEG video to browser
             */
            class Server {
                public:
                    Exception exception;
                    HttpServerThread httpServerThread;

                    /**
                     * Constructor
                     */
                    Server(uint8_t port = 80) :
                        exception("MjpegStreamServer"),
                        httpServerThread("MjpegStreamServer", port) {

                        }

                    /**
                     * Start server
                     */
                    Exception& begin(uint16_t port = 80) {
                        if (!wifiSta.isConnected())
                            return exception.set("WiFi not connected");

                        httpServerThread.httpServer.setPort(port);

                        // render index HTML
                        httpServerThread.httpServer.on("/", [this]() {
                            httpServerThread.httpServer.webServer.send(200, "text/html", "<img src=\"/mjpeg\">");
                        });

                        // serve MJPEG stream
                        httpServerThread.httpServer.addMultipartGetHandler("/mjpeg", [this](WiFiClient client) {
                            sendMjpeg(client);
                        });

                        // serve JPEG frame
                        httpServerThread.httpServer.addGetHandlerWithTest(
                            // route
                            "/jpeg",
                            // test
                            []() { return camera.capture().isOk(); },
                            // error message on fail
                            "Cannot capture frame",
                            // success function
                            [this]() { sendJpeg(); }
                        );

                        // run in thread
                        httpServerThread.thread.withStackSize(MJPEG_SERVER_STACK_SIZE);

                        if (!httpServerThread.begin().isOk())
                            return httpServerThread.exception;

                        return exception.clear();
                    }

                protected:
                    /**
                     * 
                     */
                    void sendJpeg() {
                        ESP_LOGI("MjpegStreamServer", "/jpeg");

                        WiFiClient client = httpServerThread.httpServer.webServer.client();

                        client.println(F("HTTP/1.1 200 OK"));
                        client.println(F("Content-Type: image/jpeg"));
                        client.println(F("Access-Control-Allow-Origin: *"));
                        client.print(F("Content-Length: "));
                        client.println((unsigned int) camera.getSizeInBytes());
                        client.println();
                        client.write((const char *) camera.frame->buf, camera.getSizeInBytes());
                    }

                    /**
                     * 
                     */
                    template<typename Client>
                    void sendMjpeg(Client client) {
                        ESP_LOGI("MjpegStreamServer", "/mjpeg");

                        if (!camera.capture().isOk()) {
                            httpServerThread.httpServer.abortByServer(camera.exception.toString());
                            return;
                        }

                        // todo
                    }
            };
        }
    }
}

namespace e {
    static Eloquent::Esp32cam::Mjpeg::Server mjpegServer;
}

#endif