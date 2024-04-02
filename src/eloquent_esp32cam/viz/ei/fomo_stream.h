#pragma once

#ifndef ELOQUENT_ESP32CAM_VIZ_EI_FOMO_STREAM_H
#define ELOQUENT_ESP32CAM_VIZ_EI_FOMO_STREAM_H

#include "../../camera/Camera.h"
#include "../../extra/exception.h"
#include "../../extra/esp32/http/server.h"
#include "../../edgeimpulse/fomo.h"
#include "../mjpeg.h"

using eloq::camera;
using eloq::viz::mjpeg;
using eloq::ei::fomo;
using Eloquent::Error::Exception;
using Eloquent::Extra::Esp32::Http::HttpServer;
using OnObjectsCallback = std::function<void()>;

namespace Eloquent {
    namespace Esp32cam {
        namespace Viz {
            namespace EdgeImpulse {
                /**
                 * Show FOMO results in web browser
                 * overlayed over MJPEG stream
                 */
                class FOMOStream {
                    public:
                        Exception exception;
                        HttpServer server;

                        /**
                         * Constructor
                         */
                        FOMOStream() :
                            exception("FOMO Stream"),
                            server("FOMO Stream") {

                            }

                        /**
                         * Debug self IP address
                         */
                        String address() const {
                            return String("FOMO stream is available at http://") + wifi.ip();
                        }

                        /**
                         * Start server
                         */
                        Exception& begin() {
                            if (!mjpeg.begin().isOk())
                                return exception.propagate(mjpeg);

                            onIndex();
                            onEventStream();

                            return server.beginInThread(exception);
                        }

                        /**
                         * Register callback to run when something is detected
                         */
                         void whenYouSeeAnything(OnObjectsCallback callback) {
                             _onObjects = callback;
                         }

                    protected:
                        OnObjectsCallback _onObjects;

                        /**
                         * Display main page
                         */
                        void onIndex() {
                            server.onGET("/", [this](WebServer *web) {
                                static const uint8_t index[4851] = {31, 139, 8, 0, 0, 0, 0, 0, 0, 19, 149, 87, 109, 111, 226, 56, 16, 254, 222, 95, 97, 193, 158, 146, 116, 33, 105, 87, 251, 225, 196, 2, 171, 147, 110, 43, 157, 180, 167, 158, 218, 187, 79, 171, 213, 53, 56, 19, 226, 214, 177, 35, 219, 1, 34, 142, 255, 126, 126, 73, 10, 180, 129, 132, 145, 90, 96, 226, 121, 230, 153, 241, 204, 216, 153, 102, 42, 167, 243, 43, 164, 101, 154, 65, 156, 184, 175, 246, 167, 34, 138, 194, 252, 238, 254, 207, 123, 244, 168, 4, 196, 57, 122, 4, 177, 2, 49, 141, 220, 147, 253, 74, 137, 5, 41, 20, 146, 2, 207, 6, 153, 82, 133, 156, 68, 17, 78, 88, 168, 98, 66, 215, 132, 37, 88, 202, 16, 243, 124, 48, 159, 70, 110, 105, 237, 48, 218, 123, 156, 46, 120, 82, 161, 205, 56, 137, 85, 60, 27, 252, 86, 20, 126, 48, 64, 152, 198, 82, 206, 6, 41, 133, 13, 34, 10, 114, 57, 198, 192, 20, 8, 244, 92, 74, 69, 210, 170, 254, 57, 56, 160, 146, 144, 85, 99, 38, 128, 198, 138, 172, 224, 224, 177, 93, 66, 242, 37, 154, 88, 174, 249, 115, 1, 203, 1, 138, 222, 44, 208, 158, 10, 109, 10, 154, 78, 202, 197, 108, 176, 88, 112, 237, 159, 33, 243, 9, 114, 128, 38, 47, 80, 57, 109, 72, 146, 55, 232, 13, 137, 247, 90, 251, 164, 104, 200, 197, 11, 201, 105, 169, 125, 16, 70, 9, 131, 241, 130, 114, 252, 130, 214, 227, 207, 40, 211, 127, 11, 46, 18, 16, 227, 79, 205, 151, 56, 95, 232, 255, 159, 111, 110, 208, 88, 137, 152, 73, 195, 110, 188, 25, 223, 70, 159, 14, 21, 149, 85, 8, 94, 178, 4, 146, 113, 90, 82, 170, 185, 74, 85, 81, 152, 13, 182, 20, 82, 53, 65, 79, 31, 182, 150, 55, 222, 236, 126, 121, 26, 33, 197, 139, 3, 93, 165, 117, 59, 179, 73, 69, 23, 251, 140, 36, 9, 48, 212, 30, 132, 227, 124, 58, 134, 147, 164, 90, 56, 85, 86, 181, 38, 137, 202, 246, 202, 181, 85, 102, 64, 150, 217, 129, 117, 118, 134, 253, 52, 122, 183, 37, 186, 138, 235, 109, 62, 168, 30, 183, 172, 189, 176, 163, 168, 100, 197, 203, 210, 20, 114, 20, 211, 66, 199, 251, 172, 107, 33, 129, 20, 196, 155, 186, 62, 48, 61, 246, 105, 122, 129, 175, 195, 191, 97, 163, 238, 68, 156, 67, 221, 86, 51, 148, 150, 12, 43, 194, 153, 15, 44, 41, 56, 97, 106, 132, 56, 179, 75, 2, 180, 125, 23, 141, 202, 136, 12, 229, 59, 219, 182, 165, 70, 40, 40, 180, 40, 83, 205, 83, 175, 246, 188, 171, 214, 69, 41, 40, 156, 189, 186, 15, 90, 215, 24, 9, 85, 6, 204, 23, 32, 209, 108, 126, 194, 95, 35, 152, 51, 169, 144, 102, 153, 88, 207, 218, 38, 52, 77, 30, 46, 65, 61, 88, 165, 127, 218, 205, 222, 62, 1, 204, 29, 0, 131, 53, 50, 169, 251, 221, 105, 122, 89, 11, 168, 13, 31, 96, 249, 109, 83, 248, 94, 120, 253, 117, 56, 124, 188, 191, 27, 14, 253, 240, 227, 215, 96, 56, 252, 230, 190, 95, 7, 222, 8, 121, 210, 11, 218, 211, 211, 136, 139, 38, 52, 31, 126, 224, 114, 209, 228, 31, 21, 101, 94, 248, 219, 132, 51, 24, 161, 85, 76, 75, 216, 157, 218, 145, 67, 33, 41, 242, 141, 205, 249, 104, 246, 254, 85, 41, 216, 151, 243, 36, 141, 212, 27, 254, 113, 214, 36, 48, 116, 159, 190, 101, 166, 169, 115, 93, 124, 132, 45, 253, 142, 128, 141, 68, 145, 77, 161, 25, 205, 200, 244, 57, 74, 205, 136, 233, 21, 153, 128, 80, 129, 84, 190, 163, 19, 244, 73, 136, 17, 83, 178, 63, 254, 29, 161, 212, 180, 192, 200, 148, 142, 250, 105, 75, 40, 132, 13, 224, 6, 173, 155, 185, 17, 37, 170, 158, 94, 27, 210, 214, 107, 168, 211, 147, 235, 61, 166, 192, 150, 42, 67, 115, 116, 211, 111, 135, 26, 169, 251, 247, 8, 172, 31, 194, 14, 225, 88, 55, 35, 242, 77, 243, 239, 250, 5, 249, 218, 223, 38, 85, 157, 22, 61, 64, 245, 158, 11, 192, 165, 144, 208, 185, 212, 213, 100, 91, 107, 152, 142, 8, 190, 156, 5, 216, 157, 206, 201, 137, 71, 187, 171, 246, 95, 151, 205, 96, 125, 195, 232, 154, 157, 117, 92, 237, 181, 99, 175, 14, 250, 240, 49, 215, 29, 125, 219, 249, 176, 173, 113, 245, 9, 24, 27, 196, 48, 227, 82, 237, 38, 191, 222, 70, 79, 163, 86, 0, 119, 153, 152, 160, 31, 63, 71, 237, 187, 65, 24, 81, 39, 103, 186, 17, 55, 225, 94, 207, 129, 102, 60, 30, 156, 44, 190, 23, 193, 74, 95, 145, 164, 158, 109, 174, 16, 131, 238, 177, 109, 79, 23, 199, 206, 100, 200, 88, 117, 150, 64, 40, 11, 170, 217, 122, 255, 121, 221, 37, 30, 230, 113, 225, 219, 49, 210, 73, 229, 56, 210, 34, 214, 92, 100, 29, 233, 63, 15, 223, 31, 33, 22, 56, 251, 203, 106, 45, 96, 207, 129, 96, 90, 220, 97, 153, 163, 200, 247, 64, 8, 46, 188, 158, 205, 105, 164, 46, 12, 195, 138, 83, 61, 146, 140, 125, 59, 226, 5, 51, 253, 130, 25, 229, 145, 196, 155, 152, 116, 72, 248, 131, 169, 35, 207, 250, 73, 16, 180, 23, 92, 43, 210, 70, 3, 221, 234, 107, 229, 181, 131, 187, 163, 60, 62, 6, 220, 92, 134, 87, 117, 225, 85, 151, 225, 173, 187, 240, 214, 151, 225, 101, 93, 120, 217, 101, 120, 184, 51, 129, 248, 194, 12, 226, 206, 20, 226, 254, 57, 220, 117, 31, 5, 61, 26, 54, 37, 84, 191, 101, 249, 246, 53, 72, 247, 172, 187, 131, 159, 63, 17, 119, 103, 106, 223, 77, 172, 250, 2, 123, 226, 22, 247, 158, 120, 191, 177, 63, 141, 204, 21, 115, 126, 165, 95, 45, 205, 123, 237, 255, 36, 41, 195, 187, 222, 14, 0, 0};
                                server.sendGzip(index, 4851);
                            });
                        }


                        /**
                         * 
                         */
                        void onEventStream() {
                            server.onStream("/events", [this](WebServer *web, WiFiClient *client) {
                                if (!fomo.run().isOk()) {
                                    delay(100);
                                    client->print("err=");
                                    client->print(fomo.exception.toString());
                                    return;
                                }

                                if (!fomo.found()) {
                                    client->print("status=No objects found");
                                    return;
                                }

                                fomo.forEach([client](int i, eloq::ei::bbox_t bbox) {
                                    ESP_LOGI(
                                        "FOMO", 
                                        "id=%d,x=%d,y=%d,w=%d,h=%d,cx=%d,cy=%d",
                                        i, 
                                        bbox.x, 
                                        bbox.y, 
                                        bbox.width, 
                                        bbox.height,
                                        bbox.cx,
                                        bbox.cy
                                    );
                                    client->print("id=");
                                    client->print(i);
                                    client->print("&x=");
                                    client->print(((float) bbox.x) / EI_CLASSIFIER_INPUT_WIDTH);
                                    client->print("&y=");
                                    client->print(((float) bbox.y) / EI_CLASSIFIER_INPUT_HEIGHT);
                                    client->print("&w=");
                                    client->print(((float) bbox.width) / EI_CLASSIFIER_INPUT_WIDTH);
                                    client->print("&h=");
                                    client->print(((float) bbox.height) / EI_CLASSIFIER_INPUT_HEIGHT);
                                    client->print("&cx=");
                                    client->print(((float) bbox.cx) / EI_CLASSIFIER_INPUT_WIDTH);
                                    client->print("&cy=");
                                    client->print(((float) bbox.cy) / EI_CLASSIFIER_INPUT_HEIGHT);
                                    client->print("|");
                                });

                                if (_onObjects)
                                    _onObjects();
                            }, 7000);
                        }
                };
            }
        }
    }
}

namespace eloq {
    namespace viz {
        namespace ei {
            static Eloquent::Esp32cam::Viz::EdgeImpulse::FOMOStream fomoStream;
        }
    }
}

#endif