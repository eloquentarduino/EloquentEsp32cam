#ifndef ELOQUENT_ESP32CAM_VIZ_FACE_DETECTION_H
#define ELOQUENT_ESP32CAM_VIZ_FACE_DETECTION_H

#include "../camera/Camera.h"
#include "../extra/exception.h"
#include "../extra/esp32/wifi/sta.h"
#include "../extra/esp32/http/server.h"
#include "../face/detection.h"
#include "./mjpeg.h"

using namespace eloq;
using Eloquent::Extra::Exception;
using Eloquent::Extra::Esp32::Http::HttpServer;
using Eloquent::Esp32cam::Face::FaceDetection;


namespace Eloquent {
    namespace Esp32cam {
        namespace Viz {
            /**
             * Show face detection result in browser
             */
            class FaceDetectionServer {
                public:
                    Exception exception;
                    HttpServer server;

                    /**
                     * Constructor
                     */
                    FaceDetectionServer() :
                        exception("FaceDetectionServer"),
                        server("FaceDetectionServer") {

                        }

                    /**
                     * Debug self IP address
                     */
                    String address() const {
                        return String("Face Detection stream is available at http://") + wifi.ip();
                    }

                    /**
                     * Start server
                     */
                    Exception& begin() {
                        if (!viz::mjpeg.begin().isOk())
                            return exception.propagate(viz::mjpeg);

                        onIndex();
                        onEvents();

                        // run in thread
                        server.thread.withStackSize(5000);

                        if (!server.begin().isOk())
                            return exception.propagate(server);

                        return exception.clear();
                    }

                protected:

                    /**
                     * Display main page
                     */
                    void onIndex() {
                        server.onGET("/", [this](WebServer *web) {
                            WiFiClient client = web->client();
                            static const uint8_t index[4358] = {31, 139, 8, 0, 0, 0, 0, 0, 0, 19, 165, 23, 77, 143, 155, 70, 244, 238, 95, 241, 74, 35, 25, 18, 118, 216, 84, 61, 84, 174, 113, 85, 105, 219, 67, 15, 137, 148, 237, 109, 181, 202, 98, 120, 192, 236, 226, 25, 196, 140, 191, 228, 248, 191, 103, 222, 12, 94, 19, 199, 216, 68, 121, 7, 198, 12, 239, 251, 219, 211, 82, 47, 170, 217, 8, 12, 76, 75, 76, 50, 247, 211, 190, 106, 174, 43, 156, 253, 155, 164, 8, 119, 168, 49, 213, 92, 10, 184, 199, 102, 133, 205, 52, 114, 31, 143, 200, 42, 109, 120, 173, 65, 53, 105, 236, 149, 90, 215, 106, 18, 69, 105, 38, 152, 78, 120, 181, 230, 34, 75, 149, 98, 169, 92, 120, 179, 105, 228, 80, 91, 153, 209, 81, 232, 116, 46, 179, 45, 108, 110, 178, 68, 39, 177, 247, 119, 93, 251, 129, 215, 145, 144, 241, 21, 164, 85, 162, 84, 236, 53, 88, 37, 154, 175, 176, 243, 217, 162, 240, 69, 1, 19, 171, 194, 226, 185, 198, 194, 131, 232, 4, 161, 110, 208, 8, 208, 184, 209, 177, 247, 223, 253, 199, 15, 76, 233, 134, 139, 130, 231, 91, 63, 55, 118, 170, 128, 244, 51, 72, 39, 100, 26, 23, 181, 145, 72, 180, 185, 108, 98, 111, 62, 151, 27, 224, 2, 232, 68, 229, 193, 228, 5, 183, 238, 150, 109, 78, 116, 114, 98, 15, 138, 207, 101, 147, 97, 3, 238, 184, 249, 237, 240, 35, 89, 204, 205, 243, 247, 219, 91, 72, 230, 74, 86, 75, 35, 137, 139, 138, 11, 188, 153, 87, 50, 125, 49, 2, 148, 222, 86, 24, 123, 187, 10, 115, 61, 129, 167, 55, 59, 39, 108, 95, 111, 158, 66, 208, 178, 62, 222, 109, 221, 221, 154, 103, 186, 60, 222, 174, 221, 109, 137, 188, 40, 59, 12, 74, 186, 222, 91, 163, 79, 76, 142, 14, 54, 119, 34, 16, 153, 16, 204, 70, 231, 99, 30, 69, 75, 81, 191, 20, 20, 227, 40, 169, 106, 163, 251, 179, 113, 76, 134, 57, 54, 39, 33, 239, 144, 126, 43, 147, 210, 68, 174, 25, 197, 229, 94, 55, 152, 44, 32, 134, 124, 41, 108, 222, 249, 40, 178, 90, 114, 161, 67, 144, 226, 206, 36, 72, 0, 187, 239, 252, 172, 75, 174, 40, 162, 39, 164, 231, 80, 9, 42, 212, 48, 95, 230, 70, 67, 131, 61, 30, 143, 206, 34, 229, 168, 211, 242, 85, 122, 112, 22, 135, 128, 233, 18, 133, 223, 160, 130, 120, 214, 35, 239, 0, 169, 20, 38, 200, 200, 42, 89, 248, 99, 203, 31, 179, 113, 63, 231, 3, 137, 6, 99, 88, 102, 149, 53, 98, 24, 21, 11, 43, 80, 127, 178, 151, 254, 16, 250, 12, 83, 233, 24, 8, 92, 195, 255, 166, 10, 238, 220, 205, 32, 106, 83, 58, 142, 240, 19, 22, 255, 108, 106, 127, 204, 222, 254, 234, 179, 119, 127, 5, 230, 249, 54, 24, 135, 48, 86, 198, 138, 139, 140, 156, 1, 140, 14, 63, 112, 30, 59, 68, 9, 234, 229, 162, 246, 119, 153, 20, 24, 194, 42, 169, 150, 184, 239, 139, 91, 23, 120, 14, 62, 209, 92, 54, 224, 40, 95, 47, 27, 113, 89, 71, 130, 54, 43, 222, 197, 7, 151, 49, 119, 250, 86, 49, 163, 185, 188, 183, 125, 195, 191, 98, 47, 65, 20, 89, 167, 81, 83, 3, 42, 105, 200, 229, 82, 100, 131, 12, 107, 144, 105, 84, 218, 119, 234, 4, 67, 252, 65, 64, 121, 253, 240, 57, 180, 210, 66, 202, 21, 253, 104, 115, 134, 225, 6, 211, 3, 179, 235, 138, 19, 232, 102, 59, 80, 40, 65, 155, 101, 100, 105, 12, 182, 189, 214, 73, 163, 208, 39, 69, 134, 5, 136, 192, 213, 183, 79, 108, 134, 17, 237, 33, 77, 76, 17, 129, 143, 198, 67, 251, 97, 134, 189, 214, 61, 185, 231, 42, 197, 0, 166, 38, 204, 13, 166, 75, 99, 238, 85, 84, 151, 133, 231, 138, 129, 106, 32, 248, 243, 34, 131, 125, 191, 79, 122, 62, 237, 71, 231, 223, 126, 172, 43, 155, 113, 124, 173, 167, 182, 118, 157, 207, 23, 59, 144, 205, 224, 161, 221, 192, 172, 6, 111, 118, 45, 95, 51, 223, 18, 226, 200, 74, 169, 244, 126, 242, 199, 251, 232, 41, 60, 223, 136, 105, 62, 79, 224, 225, 49, 60, 31, 140, 130, 186, 185, 157, 199, 189, 13, 159, 128, 234, 234, 23, 59, 40, 44, 67, 248, 242, 5, 58, 175, 238, 121, 57, 237, 90, 51, 31, 30, 251, 147, 162, 69, 33, 190, 23, 89, 57, 113, 63, 143, 194, 43, 109, 154, 56, 97, 210, 244, 161, 147, 173, 97, 6, 183, 151, 13, 97, 139, 164, 126, 37, 250, 56, 127, 54, 27, 30, 51, 91, 10, 47, 132, 189, 13, 7, 84, 190, 93, 75, 38, 131, 251, 82, 187, 187, 88, 5, 221, 238, 50, 172, 11, 181, 251, 141, 165, 219, 14, 167, 59, 238, 64, 206, 37, 195, 41, 59, 123, 146, 37, 181, 123, 210, 128, 54, 113, 241, 107, 208, 83, 159, 61, 9, 205, 5, 215, 23, 83, 217, 181, 219, 215, 125, 135, 166, 204, 113, 119, 242, 199, 17, 174, 80, 104, 101, 6, 179, 235, 165, 20, 229, 78, 226, 199, 224, 58, 108, 47, 123, 199, 184, 221, 167, 122, 54, 132, 239, 45, 30, 214, 109, 166, 17, 173, 47, 179, 145, 89, 255, 233, 239, 199, 87, 134, 43, 168, 35, 133, 12, 0, 0};

                            client.println(F("HTTP/1.1 200 OK"));
                            client.println(F("Content-Type: text/html"));
                            client.print(F("Content-Length: "));
                            client.println(sizeof(index));
                            client.println(F("Content-Encoding: gzip\r\n"));
                            client.write(index, sizeof(index));
                        });
                    }

                    /**
                     * Display main page
                     */
                    void onEvents() {
                        server.onGET("/events", [this](WebServer *web) {
                            WiFiClient client = web->client();

                            while (true) {
                                delay(1);
                                yield();

                                if (!face::detection.detect().isOk()) {
                                    continue;
                                }

                                if (!face::detection.found()) {
                                    client.println("#[]#");
                                    continue;
                                }

                                auto face = face::detection.first;
                                
                                client.print("#{\"faces\":[");
                                client.print("{\"x\":");
                                client.print(face.x);
                                client.print(",\"y\":");
                                client.print(face.y);
                                client.print(",\"w\":");
                                client.print(face.width);
                                client.print(",\"h\":");
                                client.print(face.height);
                                client.print("}");
                                /*face::detection.forEach([&client](int i, face_t face) {
                                    client.print("{\"x\":");
                                    client.print(face.x);
                                    client.print(",\"y\":");
                                    client.print(face.y);
                                    client.print(",\"w\":");
                                    client.print(face.width);
                                    client.print(",\"h\":");
                                    client.print(face.height);
                                    client.print("},");
                                });*/
                                client.println("]}#");
                                client.flush();
                            }
                        });
                    }
            };
        }
    }
}

namespace eloq {
    namespace viz {
        namespace face {
            static Eloquent::Esp32cam::Viz::FaceDetectionServer detection;
        }
    }
}

#endif