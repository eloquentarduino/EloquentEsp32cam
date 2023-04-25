//
// Created by Simone on 23/04/23.
//

#ifndef ELOQUENTESP32CAM_ISHTTPSERVER_H
#define ELOQUENTESP32CAM_ISHTTPSERVER_H


#include <WiFi.h>
#include <WebServer.h>
#include "./HasErrorMessage.h"


#define SEND_RAW_LITERAL(x) server.sendContent(F(R"===( x )==="));


namespace Eloquent {
    namespace Esp32cam {
        namespace Trait {
            /**
             * Common functions for classes that start a web server
             */
            class IsHttpServer : public HasErrorMessage {
            public:
                WebServer server;


                /**
                 * Set web server port
                 * @param port
                 */
                void setHttpPort(uint16_t port) {
                    httpPort = port;
                }

                /**
                 * Handle HTTP client
                 */
                void handle() {
                    server.handleClient();
                }

                /**
                 * Print welcome message
                 * @return
                 */
                String getWelcomeMessage() {
                    if (!camera.isConnectedToWiFi()) {
                        setErrorMessage("WiFi not connected");
                        return "WiFi not connected";
                    }

                    IPAddress ipAddr = WiFi.localIP();
                    String ip = String(ipAddr[0]) + '.' + ipAddr[1] + '.' + ipAddr[2] + '.' + ipAddr[3];
                    String more = getMoreWelcomeMessage();

                    return
                        String(getServerName())
                        + String(F(" available at http://"))
                        + ip
                        + (httpPort != 80 ? String(':') + httpPort : "")
                        + (more != "" ? "\n" + more : "");
                }

            protected:
                uint16_t httpPort = 80;

                /**
                 * Get server name for debug
                 * @return
                 */
                virtual const char* getServerName() = 0;

                /**
                 * Allow subclasses to add more messages
                 * @return
                 */
                virtual String getMoreWelcomeMessage() {
                    return "";
                }

                /**
                 * Start server
                 * @return
                 */
                bool startServer() {
                    if (!camera.isConnectedToWiFi())
                        return setErrorMessage("WiFi not connected");

                    server.begin(httpPort);

                    return true;
                }

                /**
                 * Add GET route handler
                 * @tparam Callback
                 * @param route
                 * @param callback
                 */
                template<typename Callback>
                void on(const char *route, Callback callback) {
                    ESP_LOGD("HttpServer", "Registering route GET %s::%s", getServerName(), route);
                    server.on(route, HTTP_GET, callback);
                }

                /**
                 * Add POST route handler
                 * @tparam Callback
                 * @param route
                 * @param callback
                 */
                template<typename Callback>
                void onPOST(const char *route, Callback callback) {
                    ESP_LOGD("HttpServer", "Registering route POST %s::%s", getServerName(), route);
                    server.on(route, HTTP_POST, callback);
                }

                /**
                 * Abort with server error message
                 * @param message
                 */
                void serverError(String message) {
                    server.send(500, "text/plain", message);
                }

                /**
                 * Abort with client error message
                 * @param message
                 */
                void clientError(String message) {
                    server.send(400, "text/plain", message);
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
                int argInt(const char* name, int fallback) {
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
                String arg(const char *name, const char *fallback = "") {
                    if (!server.hasArg(name))
                        return fallback;

                    return server.arg(name);
                }

                /**
                 * Render remote HTML
                 */
                void fetch() {
                    //const char *baseUrl = "http://192.168.243.152:9080/";
                    const char *baseUrl = "https://eloquentarduino.com/arduino/libraries/eloquentesp32cam/lib-assets/";

                    server.sendContent(F("<head><title>"));
                    server.sendContent(getServerName());
                    server.sendContent(F("</title></head><body><script src=\""));
                    server.sendContent(baseUrl);
                    server.sendContent(getServerName());
                    server.sendContent(F(".js\"></script></body>"));
                }

                /**
                 * On main page fetch remote content
                 */
                virtual void onIndex() {
                    fetch();
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_ISHTTPSERVER_H
