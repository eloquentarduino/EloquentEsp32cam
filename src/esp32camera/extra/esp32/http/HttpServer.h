#ifndef ELOQUENT_EXTRA_ESP32_HTTP_HTTPSERVER
#define ELOQUENT_EXTRA_ESP32_HTTP_HTTPSERVER

#include <WiFi.h>
#include <WebServer.h>
#include "../../error/Exception.h"
#include "../wifi/WifiSta.h"

using namespace e;
using Eloquent::Extra::Error::Exception;


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
                        Exception exception;

                        /**
                         * Constructor
                         */
                        HttpServer(const char* serverName, uint16_t serverPort = 80) :
                            name(serverName),
                            port(serverPort),
                            exception(serverName) {

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
                            if (!wifiSta.isConnected())
                                return exception.set("Not connected to WiFi");

                            // add /app.js endpoint, if exists
                            #ifdef AUTOLOAD_JAVASCRIPT_SPA
                            on("/app.js", [this]() {
                                webServer.send(200, "text/javascript", JAVASCRIPT_SPA);
                            });
                            #endif

                            // render Alpine.js SPA, if exists
                            #ifdef IS_ALPINE_SPA
                            on("/", [this]() {
                                webServer.send(200, "text/html", ALPINE_SPA);
                            });
                            #endif

                            webServer.begin(port);

                            return exception.clear();
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
                            webServer.on(route, HTTP_GET, handler);
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
                         * Abort with error message
                         * @param message
                         * @param contentType
                         * @param statusCode
                         */
                        void abort(String message, const char* contentType = "text/plain", uint16_t statusCode = 500) {
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