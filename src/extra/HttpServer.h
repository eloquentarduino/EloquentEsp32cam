#pragma once

#include <WebServer.h>


namespace Eloquent {
    namespace Extra {

        /**
         * Improved http server creation
         */
        class HttpServer {
            public:
                WebServer server;

                /**
                 * Constructor
                 */
                HttpServer() :
                    _port(80) {

                    }

                /**
                 * 
                 */
                String getAddress() {
                    IPAddress ipAddr = WiFi.localIP();
                    String ip = String(ipAddr[0]) + '.' + ipAddr[1] + '.' + ipAddr[2] + '.' + ipAddr[3];

                    if (_port == 80)
                        return ip;

                    return ip + ':' + String(_port);
                }

                /**
                 * Set web server port
                 * @param port
                 */
                void setHttpPort(uint16_t port) {
                    _port = port;
                }

                /**
                 * Start server
                 * @return
                 */
                bool start() {
                    server.begin(_port);

                    return true;
                }

                /**
                 * Run server in thread
                 * @param thread
                 */
                template<typename Thread>
                bool startInThread(Thread thread) {
                    thread
                        .withDefaultArgs((void*) &server)
                        .run([](void *args) {
                            WebServer *server = (WebServer*) args;

                            while (true) {
                                server->handleClient();
                                yield();
                            }
                        });

                    return start();
                }

                /**
                 * Handle HTTP client
                 */
                virtual void handle() {
                    server.handleClient();
                }

                /**
                 * Add GET route handler (alias to onGET)
                 * @tparam Handler
                 * @param route
                 * @param handler
                 */
                template<typename Handler>
                void on(const char *route, Handler handler) {
                    onGET(route, handler);
                }

                /**
                 * Add GET route handler
                 * @tparam Handler
                 * @param route
                 * @param handler
                 */
                template<typename Handler>
                void onGET(const char *route, Handler handler) {
                    ESP_LOGD("HttpServer", "Registering route GET %s::%s", getServerName(), route);
                    server.on(route, HTTP_GET, handler);
                }

                /**
                 * Add POST route handler
                 * @tparam Handler
                 * @param route
                 * @param handler
                 */
                template<typename Handler>
                void onPOST(const char *route, Handler handler) {
                    ESP_LOGD("HttpServer", "Registering route POST %s::%s", getServerName(), route);
                    server.on(route, HTTP_POST, handler);
                }

                /**
                 * Abort with error message
                 * @param message
                 * @param contentType
                 * @param statusCode
                 */
                void abort(String message, const char* contentType = "text/plain", uint16_t statusCode = 500) {
                    server.send(statusCode, contentType, message);
                }

                /**
                 * Abort with server error message
                 * @param message
                 * @param contentType
                 */
                void abortByServer(String message, const char* contentType = "text/plain") {
                    abort(message, contentType, 500);
                }

                /**
                 * Abort with client error message
                 * @param message
                 * @param contentType
                 */
                void abortByClient(String message, const char* contentType = "text/plain") {
                    abort(message, contentType, 400);
                }

                /**
                 * Return success response
                 */
                void ok() {
                    server.send(200, "text/plain", "OK");
                }

                /**
                 * Stop condition
                 */
                void sendChunks() {

                }

                /**
                 * Send each of the arguments
                 * @tparam T
                 * @tparam Args
                 * @param first
                 * @param args
                 */
                template<typename T, typename... Args>
                void sendChunks(T first, Args... args) {
                    server.sendContent(String(first));
                    sendChunks(args...);
                }

                /**
                 * Get int argument
                 * @param name
                 * @param fallback
                 * @return
                 */
                int getIntArg(const char* name, int fallback = 0) {
                    if (!server.hasArg(name))
                        return fallback;

                    return server.arg(name).toInt();
                }

                /**
                 *
                 * @param name
                 * @param fallback
                 * @return
                 */
                String getArg(const char *name, String fallback = "") {
                    if (!server.hasArg(name))
                        return fallback;

                    return server.arg(name);
                }

            protected:
                uint16_t _port;
        };
    }
}
