#ifndef ELOQUENTESP32CAM_MOTION_DETECTION_WEB_SOCKET
#define ELOQUENTESP32CAM_MOTION_DETECTION_WEB_SOCKET

#ifndef MOTION_DETECTION_WEB_SOCKET_PORT
#define MOTION_DETECTION_WEB_SOCKET_PORT 82
#endif

#ifndef MOTION_DETECTION_WEBSOCKET_STACK_SIZE
#define MOTION_DETECTION_WEBSOCKET_STACK_SIZE 15000
#endif

#include "../../extra/esp32/WebSocketThread.h"
#include "./MotionDetection.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Stream motion detection data over WebSockets
             */
            class MotionDetectionWebSocket {
            public:
                Eloquent::Extra::Esp32::WebSocketThread webSocketThread;

                /**
                 * 
                 */
                MotionDetectionWebSocket() :
                    connected(false),
                    webSocketThread("MotionDetectionWebSocket", MOTION_DETECTION_WEB_SOCKET_PORT) {

                    }

                /**
                 * Test if websocket is connected
                 */
                inline bool isConnected() {
                    return connected;
                }

                /**
                 * Start WebSocket server
                 */
                bool begin() {
                    webSocketThread
                        .withStackSize(MOTION_DETECTION_WEBSOCKET_STACK_SIZE)
                        .begin([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
                            // on websocket connection, continously stream motion background + mask
                            if (type == WStype_CONNECTED) {
                                connected = true;
                                delay(2000);

                                while (true) {
                                    if (motionDetection.isOk()) {
                                        const uint16_t width = jpeg.getDecodedWidth();
                                        const uint16_t height = jpeg.getDecodedHeight();
                                        
                                        Eloquent::Extra::JsonBuilder json(100);

                                        json.object();
                                        json.add("width", width);
                                        json.add("height", height);
                                        json.add("detected", motionDetection.detected());
                                        json.add("benchmark", motionDetection.benchmark.inMillis());
                                        json.add("mask", motionDetection.foregroundMask.array.toASCII());
                                        json.prop("background");
                                        json.raw('"');

                                        webSocketThread.webSocket.sendTXT(num, json.toString());
                                        sendBackground(num, width, height);
                                        webSocketThread.webSocket.sendTXT(num, "\"}\n");
                                    }
                                }
                            }
                        });

                    return true;
                }

            protected:
                bool connected;
            };
        }
    }
}

static Eloquent::Esp32cam::Motion::MotionDetectionWebSocket motionDetectionWebSocket;

#endif