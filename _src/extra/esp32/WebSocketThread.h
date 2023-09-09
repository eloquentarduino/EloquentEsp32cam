#pragma once

#include <WebSocketsServer.h>
#include "./Thread.h"


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            /**
             * Run WebSockets in thread
             */
            class WebSocketThread {
                public:
                    WebSocketsServer webSocket;
                    Eloquent::Extra::Esp32::Thread thread;

                    /**
                     * 
                     */
                    WebSocketThread(const char* threadName = "WebSocket", const uint8_t port = 82) :
                        thread(threadName),
                        stackSize(1000),
                        webSocket(port) {

                    }

                    /**
                     * Set stack size
                     */
                    WebSocketThread& withStackSize(uint16_t stackSize) {
                        thread.withStackSize(stackSize);

                        return *this;
                    }

                    /**
                     * Start thread with WebSocket request handler
                    */
                    template<typename Handler>
                    void begin(Handler handler) {
                        webSocket.begin();
                        webSocket.onEvent(handler);

                        thread
                            .withArgs((void*) &webSocket)
                            .withStackSize(stackSize)
                            .run([](void *args) {
                                WebSocketsServer *webSocket = (WebSocketsServer*) args;

                                while (true) {
                                    webSocket->loop();
                                    yield();
                                }
                            });
                    }

                protected:
                    uint16_t stackSize;
            };
        }
    }
}