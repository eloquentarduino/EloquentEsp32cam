#ifndef ELOQUENT_EXTRA_ESP32_HTTP_HTTPSERVER
#define ELOQUENT_EXTRA_ESP32_HTTP_HTTPSERVER

#include <WiFi.h>
#include <WebServer.h>
#include "../../exception.h"
#include "../wifi/sta.h"
#include "../multiprocessing/thread.h"

using namespace eloq;
using Eloquent::Error::Exception;
using Eloquent::Extra::Esp32::Multiprocessing::Thread;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Http {
                /**
                 * Improved http server creation
                 */
                class HttpServer {
                    public:
                        WebServer webServer;
                        Thread thread;
                        Exception exception;

                        /**
                         * Constructor
                         */
                        HttpServer(const char* serverName, uint16_t serverPort = 80) :
                            port(serverPort),
                            name(serverName),
                            exception(serverName),
                            thread(serverName) {

                            }

                        /**
                         * Get IP address + port
                         */
                        String getAddress() {
                            IPAddress ipAddr = WiFi.localIP();
                            String ip = String(ipAddr[0]) + '.' + ipAddr[1] + '.' + ipAddr[2] + '.' + ipAddr[3];

                            if (port == 80)
                                return ip;

                            return ip + ':' + String(port);
                        }

                        /**
                         * Convert relative path to absolute
                         */
                        String getAbsolutePath(String relativePath) {
                            return getAddress() + '/' + relativePath;
                        }

                        /**
                         * Set web server port
                         * @param httpPort
                         */
                        void setPort(uint16_t httpPort) {
                            port = httpPort;
                        }

                        /**
                         * Start server
                         * @return
                         */
                        Exception& begin() {
                            // run server in thread
                            thread
                                .withArgs((void*) this)
                                .run([](void *args) {
                                    HttpServer *httpServer = (HttpServer*) args;

                                    while (true) {
                                        httpServer->handle();
                                        yield();
                                    }
                                });

                            webServer.begin(port);

                            return exception.clear();
                        }

                        /**
                         * 
                         */
                        Exception& beginInThread(Exception& ex) {
                            thread.withStackSize(5000);

                            if (!begin().isOk())
                                return ex.propagate(*this);

                            return ex.clear();
                        }

                        /**
                         * Handle HTTP client
                         */
                        virtual void handle() {
                            webServer.handleClient();
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
                            ESP_LOGI("HttpServer", "Registering route GET %s::%s", name, route);

                            webServer.on(route, HTTP_GET, [this, handler]() {
                                handler(&webServer);
                            });
                        }

                        /**
                         * Add GET stream route handler
                         * @tparam Handler
                         * @param route
                         * @param handler
                         */
                        template<typename Handler>
                        void onStream(const char *route, Handler handler, size_t warmupTime = 0) {
                            ESP_LOGI("HttpServer", "Registering route GET stream %s::%s", name, route);

                            webServer.on(route, HTTP_GET, [this, handler, warmupTime]() {
                                WiFiClient client = webServer.client();

                                if (warmupTime > 0)
                                    delay(warmupTime);

                                while (true) {
                                    delay(5);
                                    yield();
                                    client.print("##SOF##");
                                    handler(&webServer, &client);
                                    client.print("##EOF##");
                                    client.flush();
                                }
                            });
                        }

                        /**
                         * Add POST route handler
                         * @tparam Handler
                         * @param route
                         * @param handler
                         */
                        template<typename Handler>
                        void onPOST(const char *route, Handler handler) {
                            ESP_LOGI("HttpServer", "Registering route POST %s::%s", name, route);
                            webServer.on(route, HTTP_POST, handler);
                        }

                        /**
                         * Add GET route to be run only if test passes
                         */
                        template<typename Test, typename Handler>
                        void addGetHandlerWithTest(const char* route, Test test, const char* errorMessage, Handler handler) {
                            onGET(route, [this, test, handler, errorMessage, route]() {                                
                                if (!test()) {
                                    ESP_LOGE("HttpServer", "Test did not pass for route %s", route);
                                    abortByServer(errorMessage);
                                }

                                handler();
                            });
                        }

                        /**
                         * Handle multipart requests
                         */
                        template<typename Handler>
                        void addMultipartGetHandler(const char* route, Handler handler) {
                            on(route, [this, handler]() {
                                WiFiClient client = webServer.client();

                                client.println(F("HTTP/1.1 200 OK"));
                                client.println(F("Content-Type: multipart/x-mixed-replace;boundary=frame"));
                                client.println(F("Access-Control-Allow-Origin: *"));
                                client.println(F("\r\n--frame"));

                                while (true) {
                                    if (!client.connected())
                                        continue;

                                    handler(client);

                                    client.println(F("\r\n--frame"));
                                    delay(1);
                                    yield();
                                }
                            });
                        }

                        /**
                         * Stream text of unknown length from a generator function
                         */
                        template<typename TextGenerator>
                        void textStream(TextGenerator textGenerator) {
                            webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
                            webServer.send(200, "text/plain", "");
                            textGenerator(webServer);
                            webServer.sendContent("");
                        }

                        /**
                         * Send Gzipped content
                         */
                        void sendGzip(const uint8_t* contents, const size_t length) {
                            WiFiClient client = webServer.client();

                            client.println(F("HTTP/1.1 200 OK"));
                            client.println(F("Content-Type: text/html"));
                            client.print(F("Content-Length: "));
                            client.println(length);
                            client.println(F("Content-Encoding: gzip\r\n"));
                            client.write(contents, length);
                            client.flush();
                        }

                        /**
                         * Abort with error message
                         * @param message
                         * @param contentType
                         * @param statusCode
                         */
                        void abort(String message, const char* contentType = "text/plain", uint16_t statusCode = 500) {
                            const char *msg = message.c_str();

                            ESP_LOGE("HttpServer", "Aborting request (%d): %s", statusCode, msg);

                            webServer.send(statusCode, contentType, message);
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
                         * Abort with server error message
                         * @param exception
                         * @param contentType
                         */
                        void abortByServer(Exception& exception, const char* contentType = "text/plain") {
                            abortByServer(exception.toString(), contentType);
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
                         * Abort with server error message
                         * 
                         * @param message
                         * @param contentType
                         */
                        void serverError(String message, const char* contentType = "text/plain") {
                            abort(message, contentType, 500);
                        }

                        /**
                         * Return success response
                         */
                        void ok(String message = "OK") {
                            webServer.send(200, "text/plain", message);
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
                            webServer.sendContent(String(first));
                            sendChunks(args...);
                        }

                        /**
                         * Check if argument is present in the request
                         */
                        bool hasArg(const char* name) {
                            return webServer.hasArg(name);
                        }

                        /**
                         * Get int argument
                         * @param name
                         * @param fallback
                         * @return
                         */
                        int getIntArg(const char* name, int fallback = 0) {
                            if (!hasArg(name))
                                return fallback;

                            return webServer.arg(name).toInt();
                        }

                        /**
                         *
                         * @param name
                         * @param fallback
                         * @return
                         */
                        String getArg(const char *name, String fallback = "") {
                            if (!hasArg(name))
                                return fallback;

                            return webServer.arg(name);
                        }

                    protected:
                        const char* name;
                        uint16_t port;
                };
            }
        }
    }
}

#endif