//
// Created by Simone on 05/07/23.
//

#ifndef ELOQUENTESP32CAM_MOTIONDEBUGGER_H
#define ELOQUENTESP32CAM_MOTIONDEBUGGER_H

#ifndef MOTION_DEBUGGER_STACK_SIZE
#define MOTION_DEBUGGER_STACK_SIZE 6000
#endif

#ifndef MOTION_DEBUGGER_WEBSOCKET_STACK_SIZE
#define MOTION_DEBUGGER_WEBSOCKET_STACK_SIZE 10000
#endif

#include <WebSocketsServer.h>
#include "./MotionDetection.h"
#include "../../traits/IsHttpServer.h"
#include "../../extra/Thread.h"
#include "../http/MjpegStream.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {

            /**
             *
             */
            class MotionDebugger : public Trait::IsHttpServer {
            public:
                WebSocketsServer webSocket;

                /**
                 *
                 */
                MotionDebugger() :
                        webSocket(82),
                        _mainThread("MotionDebuggerHttp", MOTION_DEBUGGER_STACK_SIZE),
                        _webSocketThread("MotionDebuggerWebSocketThread", MOTION_DEBUGGER_WEBSOCKET_STACK_SIZE) {

                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    if (!mjpegStream.begin(81))
                        return setErrorMessage(mjpegStream.getErrorMessage());

                    // index page
                    on("/", [this]() {
                        html.open([this]() {
                            html.image(":81/mjpeg");
                        });
                    });

                    setupWebsocket();

                    return startServer(_mainThread);
                }

            protected:
                Eloquent::Extra::Thread _mainThread;
                Eloquent::Extra::Thread _webSocketThread;

                /**
                 * Get server name
                 * @return
                 */
                const char * getServerName() override {
                    return "MotionDebugger";
                }

                /**
                 * Listen for WebSocket connections
                 */
                void setupWebsocket() {
                    webSocket.begin();
                    webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
                        // on connection, continously stream motion background + mask
                        if (type == WStype_CONNECTED) {
                            while (true) {
                                if (motionDetection.update()) {
                                    const uint16_t width = motionDetection.foregroundMask.getCols();
                                    const uint16_t height = motionDetection.foregroundMask.getRows();
                                    
                                    String txt = 
                                        String("{\"width\":") + 
                                        width +
                                        ", \"height\": " +
                                        height +
                                        ", \"detected\": " +
                                        (motionDetection.detected() ? "true" : "false") +
                                        ", \"benchmark\":" +
                                        motionDetection.benchmark.inMillis() +
                                        ", \"background\":[";

                                    webSocket.sendTXT(num, txt);

                                    // send JSON-encoded background image
                                    size_t i = 0;

                                    for (size_t y = 0; y < height; y++) {
                                        String row;

                                        // allocate space for a row of values
                                        row.reserve(4 * width + 2);

                                        for (uint16_t x = 0; x < width; x++) {
                                            row += motionDetection.background[i++];
                                            row += ',';
                                        }

                                        webSocket.sendTXT(num, row);
                                    }

                                    webSocket.sendTXT(num, "]}\n");
                                }
                            }
                        }
                    });

                    _webSocketThread
                        .withArgs((void*) &webSocket)
                        .run([](void *args) {
                            WebSocketsServer *webSocket = (WebSocketsServer*) args;

                            while (true) {
                                webSocket->loop();
                                yield();
                            }
                        });
                }
            };
        }
    }
}


static Eloquent::Esp32cam::Motion::MotionDebugger motionDebugger;

#endif //ELOQUENTESP32CAM_MOTIONDEBUGGER_H
