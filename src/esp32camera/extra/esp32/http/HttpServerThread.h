#ifndef ELOQUENT_EXTRA_ESP32_HTTP_HTTPSERVERTHREAD
#define ELOQUENT_EXTRA_ESP32_HTTP_HTTPSERVERTHREAD


#include "./HttpServer.h"
#include "../multiprocessing/Thread.h"
#include "../../error/Exception.h"

using Eloquent::Extra::Error::Exception;
using Eloquent::Extra::Esp32::Multiprocessing::Thread;
using Eloquent::Extra::Esp32::Http::HttpServer;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Http {
                /**
                 * Run HTTP server in a thread
                 */
                class HttpServerThread {
                public:
                    HttpServer httpServer;
                    Thread thread;
                    Exception exception;

                    /**
                     * 
                     */
                    HttpServerThread(const char* serverName, uint16_t port = 80) :
                        httpServer(serverName, port),
                        thread(serverName),
                        exception(serverName) {

                        }

                    /**
                     * Start server thread
                     */
                    Exception& begin() {
                        thread
                            .withArgs((void*) &httpServer)
                            .run([](void *args) {
                                HttpServer *httpServer = (HttpServer*) args;

                                while (true) {
                                    httpServer->handle();
                                    yield();
                                }
                            });

                        if (!httpServer.begin().isOk())
                            return exception.propagate(httpServer);

                        return exception.clear();
                    }
                };
            }
        }
    }
}

#endif