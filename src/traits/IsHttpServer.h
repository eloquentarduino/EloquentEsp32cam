//
// Created by Simone on 23/04/23.
//

#ifndef ELOQUENTESP32CAM_ISHTTPSERVER_H
#define ELOQUENTESP32CAM_ISHTTPSERVER_H


#include <WiFi.h>
#include "./HasErrorMessage.h"
#include "../extra/esp32/HttpServerThread.h"
#include "../extra/esp32/HtmlBuilder.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Trait {
            /**
             * Common functions for classes that start a web server
             */
            class IsHttpServer : public HasErrorMessage {
            public:
                Eloquent::Extra::Esp32::HttpServerThread httpServerThread;
                Eloquent::Extra::Esp32::HtmlBuilder html;

                /**
                 * Constructor
                 */
                IsHttpServer(const char* serverName) :
                    httpServerThread(serverName),
                    html(&httpServerThread.httpServer.webServer) {

                    }

                /**
                 * Handle HTTP client
                 */
                virtual void handle() {
                    httpServerThread.httpServer.handle();
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
                        + httpServerThread.httpServer.getAddress()
                        + (more != "" ? "\n" + more : "");
                }

            protected:

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
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_ISHTTPSERVER_H
