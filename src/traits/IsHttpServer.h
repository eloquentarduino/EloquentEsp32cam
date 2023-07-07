//
// Created by Simone on 23/04/23.
//

#ifndef ELOQUENTESP32CAM_ISHTTPSERVER_H
#define ELOQUENTESP32CAM_ISHTTPSERVER_H


#include <WiFi.h>
#include <WebServer.h>
#include "./HasErrorMessage.h"
#include "../extra/HttpServer.h"
#include "../extra/Thread.h"
#include "../extra/HtmlBuilder.h"


//#define SEND_RAW_LITERAL(x) server.sendContent(F(R"===( x )==="));


namespace Eloquent {
    namespace Esp32cam {
        namespace Trait {
            /**
             * Common functions for classes that start a web server
             */
            class IsHttpServer : public HasErrorMessage {
            public:
                Eloquent::Extra::HttpServer httpServer;

                /**
                 * Constructor
                 */
                IsHttpServer() :
                    html(&httpServer.server) {

                    }

                /**
                 * Handle HTTP client
                 */
                virtual void handle() {
                    httpServer.handle();
                }

                /**
                 * Print welcome message
                 * @return
                 */
                String getWelcomeMessage() {
                    if (!camera.wifi.isConnected()) {
                        setErrorMessage("WiFi not connected");
                        return "WiFi not connected";
                    }

                    String more = getMoreWelcomeMessage();

                    return
                        String(getServerName())
                        + String(F(" available at http://"))
                        + httpServer.getAddress()
                        + (more != "" ? "\n" + more : "");
                }

            protected:
                Eloquent::Extra::HtmlBuilder html;

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
                    if (!camera.wifi.isConnected())
                        return setErrorMessage("WiFi not connected");

                    return httpServer.start();
                }

                /**
                 * Start server in a thread
                 * @param thread
                 */
                bool startServer(Eloquent::Extra::Thread thread) {
                    if (!camera.wifi.isConnected())
                        return setErrorMessage("WiFi not connected");

                    return httpServer.startInThread(thread);
                }

                /**
                 * Short for httpServer.on()
                 */
                template<typename Handler>
                void on(const char* path, Handler handler) {
                    httpServer.on(path, handler);
                }

                /**
                 * Render remote HTML
                 */
                void fetch() {
                    //const char *baseUrl = "http://192.168.243.152:9080/";
                    /*const char *baseUrl = "https://eloquentarduino.com/arduino/libraries/eloquentesp32cam/lib-assets/";

                    server.sendContent(F("<head><title>"));
                    server.sendContent(getServerName());
                    server.sendContent(F("</title></head><body><script src=\""));
                    server.sendContent(baseUrl);
                    server.sendContent(getServerName());
                    server.sendContent(F(".js\"></script></body>"));*/
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_ISHTTPSERVER_H
