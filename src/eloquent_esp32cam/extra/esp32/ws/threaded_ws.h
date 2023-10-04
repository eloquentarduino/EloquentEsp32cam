#pragma once

#include <WebSocketsServer.h>
#include "../multiprocessing/Thread.h"

using Eloquent::Extra::Esp32::Multiprocessing::Thread;

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Ws {
                /**
                 * Run WebSockets in thread
                 */
                class ThreadedWs {
                    public:
                        WebSocketsServer webSocket;
                        Thread thread;

                        /**
                         * 
                         */
                        ThreadedWs(const char* threadName = "WebSocket", const uint8_t port = 82) :
                            thread(threadName),
                            stackSize(1000),
                            webSocket(port),
                            isConnected(false) {

                        }

                        /**
                         * Set stack size
                         */
                        ThreadedWs& withStackSize(uint16_t stackSize) {
                            thread.withStackSize(stackSize);

                            return *this;
                        }

                        /**
                         * Start thread with WebSocket request handler
                        */
                        template<typename Handler>
                        void begin(Handler handler) {
                            webSocket.begin();
                            webSocket.onEvent([this, handler](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
                                if (type == WStype_CONNECTED)
                                    isConnected = true;

                                if (type == WStype_DISCONNECTED)
                                    isConnected = false;

                                if (isConnected)
                                    handler(num, type, payload, length);
                            });

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
                        bool isConnected;
                };
            }
        }
    }
}