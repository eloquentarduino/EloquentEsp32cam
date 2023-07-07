#ifndef ELOQUENTESP32CAM_MJPEG_STREAM_H
#define ELOQUENTESP32CAM_MJPEG_STREAM_H


#include "../Cam.h"
#include "../../traits/IsHttpServer.h"
#include "../components/ColorJpeg.h"
#include "../../extra/Thread.h"


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
                        _thread("MjpegStreamHttp", 6000) {
                }

                /**
                 * Init server
                 * @param port
                 * @return
                 */
                bool begin(uint16_t port = 80) {
                    httpServer.setHttpPort(port);

                    on("/", [this]() {
                        html.open([this]() {
                            html.image("/mjpeg");
                        });
                    });

                    on("/mjpeg", [this]() {
                        WiFiClient client = httpServer.server.client();

                        client.println(F("HTTP/1.1 200 OK"));
                        client.println(F("Content-Type: multipart/x-mixed-replace;boundary=frame"));
                        client.println(F("Access-Control-Allow-Origin: *"));
                        client.println(F("\r\n--frame"));

                        while (true) {
                            if (!client.connected())
                                break;

                            if (!camera.capture())
                                continue;

                            // many debuggers need the decoded jpeg to work properly
                            // hooking here would be difficult, so always decode the frame
                            jpeg.decode();

                            client.println(F("Content-Type: image/jpeg"));
                            client.print(F("Content-Length: "));
                            client.println(camera.getSizeInBytes());
                            client.println();
                            client.write((const char *) camera.frame->buf, camera.getSizeInBytes());
                            client.println(F("\r\n--frame"));
                            delay(1);
                            yield();
                        }
                    });

                    on("/jpeg", [this]() {
                        if (!camera.capture()) {
                            httpServer.abortByServer("Cannot capture frame");
                            return;
                        }

                        WiFiClient client = httpServer.server.client();

                        client.println(F("HTTP/1.1 200 OK"));
                        client.println(F("Content-Type: image/jpeg"));
                        client.println(F("Access-Control-Allow-Origin: *"));
                        client.println();
                        client.write((const char *) camera.frame->buf, camera.getSizeInBytes());
                    });

                    return startServer(_thread);
                }

            protected:
                Eloquent::Extra::Thread _thread;

                /**
                 * Get server name
                 * @return
                 */
                const char * getServerName() override {
                    return "MjpegStream";
                }
            };
        }
    }
}


static Eloquent::Esp32cam::Http::MjpegStream mjpegStream;

#endif //ELOQUENTESP32CAM_MJPEG_STREAM_H
