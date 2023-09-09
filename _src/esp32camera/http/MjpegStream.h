#ifndef ELOQUENTESP32CAM_MJPEG_STREAM_H
#define ELOQUENTESP32CAM_MJPEG_STREAM_H

#ifndef MJPEG_STACK_SIZE
#define MJPEG_STACK_SIZE 5000
#endif


#include "../Cam.h"
#include "../../traits/IsHttpServer.h"



namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             * ESP32 camera MJPEG streaming web server
             */
            class MjpegStream : public Trait::IsHttpServer {
            public:

                /**
                 *
                 */
                MjpegStream() :
                    IsHttpServer("MjpegStream") {
                }

                /**
                 * Init server
                 * @param port
                 * @return
                 */
                bool begin(uint16_t port = 80) {
                    if (!camera.wifi.isConnected())
                        return setErrorMessage("WiFi not connected");

                    httpServerThread.httpServer.setHttpPort(port);

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
                        []() { return camera.capture(); },
                        // error message on fail
                        "Cannot capture frame",
                        // success function
                        [this]() { sendJpeg(); }
                    );

                    return httpServerThread.begin();
                }

            protected:
            
                /**
                 * Get server name
                 * @return
                 */
                const char * getServerName() override {
                    return "MjpegStream";
                }

                /**
                 * 
                 */
                void sendJpeg() {
                    WiFiClient client = httpServerThread.httpServer.webServer.client();

                    client.println(F("HTTP/1.1 200 OK"));
                    client.println(F("Content-Type: image/jpeg"));
                    client.println(F("Access-Control-Allow-Origin: *"));
                    client.println();
                    client.write((const char *) camera.frame->buf, camera.getSizeInBytes());
                }

                /**
                 * 
                 */
                template<typename Client>
                void sendMjpeg(Client client) {
                    if (!camera.capture())
                        return;

                    #if defined(ELOQUENTESP32CAM_JPEG_DECODER)
                    // many debuggers need the decoded jpeg to work properly
                    // hooking here would be difficult, so always decode the frame
                    // even if not stricly necessary for the MJPEG to work
                    jpeg.decode();
                    #endif

                    client.println(F("Content-Type: image/jpeg"));
                    client.print(F("Content-Length: "));
                    client.println(camera.getSizeInBytes());
                    client.println();
                    client.write((const char *) camera.frame->buf, camera.getSizeInBytes());
                }
            };
        }
    }
}


static Eloquent::Esp32cam::Http::MjpegStream mjpegStream;

#endif //ELOQUENTESP32CAM_MJPEG_STREAM_H
