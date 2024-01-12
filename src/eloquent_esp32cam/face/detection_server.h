#ifndef ELOQUENT_ESP32CAM_FACE_DETECTION_SERVER
#define ELOQUENT_ESP32CAM_FACE_DETECTION_SERVER

#include "../extra/exception.h"
#include "../extra/esp32/http/threaded_server.h"
#include "../extra/esp32/ws/threaded_ws.h"
#include "../assets/face_detection.html.h"
#include "../assets/face_detection.js.h"

using Eloquent::Error::Exception;
using Eloquent::Extra::Esp32::Http::ThreadedServer;
using Eloquent::Extra::Esp32::Ws::ThreadedWs;


namespace Eloquent {
    namespace Esp32cam {
        namespace Face {
            /**
             * Add face detection to MJPEG HTTP stream
             */
            class FaceDetectionServer {
                public:
                    Exception exception;
                    ThreadedServer http;
                    ThreadedWs ws;

                    /**
                     * 
                     */
                    FaceDetectionServer() :
                        exception("FaceDetectionWebServer"),
                        http("FaceDetection"),
                        ws("FaceDetectionWebSocket", 90) {

                        }

                    /**
                     * 
                     */
                    Exception& begin() {
                        http.thread.withStackSize(5000);

                        // HTTP endpoints
                        // render static files
                        http.router.gzip("/", Eloquent::Assets::Gz::Html::FACE_DETECTION);
                        //http.gzip("/face_detection.js", "text/javascript", Eloquent::Assets::Gz::Js::FACE_DETECTION);
                        // serve MJPEG stream
                        http.router.mjpeg();

                        if (!http.begin().isOk())
                            return exception.propagate(http);

                        /*ws.begin([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
                            Serial.println("ws event");
                        });*/

                        return exception.clear();
                    }

                protected:
                    /**
                     * 
                     */
                    template<typename Client>
                    void sendMjpeg(Client client) {
                        if (!camera.capture().isOk())
                            return;

                        client.write((const char *) camera.frame->buf, camera.getSizeInBytes());
                    }
            };
        } 
    }
}

namespace eloq {
    static Eloquent::Esp32cam::Face::FaceDetectionServer faceDetectionServer;
}

#endif