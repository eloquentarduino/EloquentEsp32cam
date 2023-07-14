#pragma once

#include <WebServer.h>


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {

            /**
             * Improved http server creation
             */
            class HttpServer {
                public:
                    WebServer webServer;

                    /**
                     * Constructor
                     */
                    HttpServer(uint16_t serverPort = 80) :
                        port(serverPort) {

                        }

                    /**
                     * 
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
                    void setHttpPort(uint16_t httpPort) {
                        port = httpPort;
                    }

                    /**
                     * Start server
                     * @return
                     */
                    bool begin() {
                        webServer.begin(port);

                        return true;
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
                        ESP_LOGD("HttpServer", "Registering route GET %s::%s", getServerName(), route);
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
                        ESP_LOGD("HttpServer", "Registering route POST %s::%s", getServerName(), route);
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
                                abort(errorMessage);
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
                        webServer.send(200, "text/plain", "OK");
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
                     * Get int argument
                     * @param name
                     * @param fallback
                     * @return
                     */
                    int getIntArg(const char* name, int fallback = 0) {
                        if (!webServer.hasArg(name))
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
                        if (!webServer.hasArg(name))
                            return fallback;

                        return webServer.arg(name);
                    }

                protected:
                    uint16_t port;
            };
        }
    }
}
