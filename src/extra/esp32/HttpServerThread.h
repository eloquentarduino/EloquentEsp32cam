#ifndef ELOQUENT_EXTRA_ESP32_HTTPSERVERTHREAD
#define ELOQUENT_EXTRA_ESP32_HTTPSERVERTHREAD


#include "./HttpServer.h"
#include "./Thread.h"


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            /**
             * Run HTTP server in a thread
             */
            class HttpServerThread {
            public:
                Eloquent::Extra::Esp32::HttpServer httpServer;
                Eloquent::Extra::Esp32::Thread thread;

                /**
                 * 
                 */
                HttpServerThread(const char* serverName, uint16_t port = 80) :
                    httpServer(port),
                    thread(serverName) {

                    }

                /**
                 * Start server thread
                 */
                bool begin() {
                    thread
                        .withArgs((void*) &httpServer)
                        .run([](void *args) {
                            HttpServer *httpServer = (HttpServer*) args;

                            while (true) {
                                httpServer->handle();
                                yield();
                            }
                        });

                    return httpServer.begin();
                }
            };
        }
    }
}

#endif