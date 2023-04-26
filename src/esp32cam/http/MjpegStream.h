#ifndef ELOQUENTESP32CAM_MJPEG_STREAM_H
#define ELOQUENTESP32CAM_MJPEG_STREAM_H


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
                 * Init server
                 * @return
                 */
                bool begin() {
                    on("/", [this]() {
                        server.send(200, "text/html", "<img src=\"/mjpeg\" />");
                    });

                    on("/mjpeg", [this]() {
                        WiFiClient client = server.client();

                        client.println(F("HTTP/1.1 200 OK"));
                        client.println(F("Content-Type: multipart/x-mixed-replace;boundary=frame"));
                        client.println(F("Access-Control-Allow-Origin: *"));
                        client.println(F("\r\n--frame"));

                        while (true) {
                            if (!camera.capture()) {
                                continue;
                            }

                            if (!client.connected())
                                break;

                            client.println(F("Content-Type: image/jpeg"));
                            client.print(F("Content-Length: "));
                            client.println(camera.getFileSize());
                            client.println();
                            client.write((const char *) camera.frame->buf, camera.getFileSize());
                            client.println(F("\r\n--frame"));
                        }
                    });

                    on("/jpeg", [this]() {
                        if (!camera.capture()) {
                            serverError("Cannot capture frame");
                            return;
                        }

                        WiFiClient client = server.client();

                        client.println(F("HTTP/1.1 200 OK"));
                        client.println(F("Content-Type: image/jpeg"));
                        client.println(F("Access-Control-Allow-Origin: *"));
                        client.println();
                        client.write((const char *) camera.frame->buf, camera.getFileSize());
                    });

                    return startServer();
                }

            protected:

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
