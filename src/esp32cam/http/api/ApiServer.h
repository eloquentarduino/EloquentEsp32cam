//
// Created by Simone on 17/02/23.
//

#ifndef ELOQUENTESP32CAM_APISERVER_H
#define ELOQUENTESP32CAM_APISERVER_H

#ifndef API_SERVER_MAX_ENDPOINTS
#define API_SERVER_MAX_ENDPOINTS 10
#endif

#include <WebServer.h>
#include "./Endpoint.h"
#include "../../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Api {
                /**
                 * Api server to group different endpoints
                 */
                class ApiServer : public HasErrorMessage {
                public:
                    WebServer webServer;

                    /**
                     *
                     * @param httpPort
                     */
                    ApiServer(uint16_t httpPort = 80) :
                        port(httpPort) {

                    }

                    /**
                     * Add endpoint
                     *
                     * @param endpoint
                     * @return
                     */
                     template<typename ConcreteEndpoint>
                    void add(ConcreteEndpoint& endpoint) {
                        welcomeMessage += '\t';
                        welcomeMessage += endpoint.registerTo(webServer);
                        welcomeMessage += '\n';
                    }

                    /**
                     * Init server
                     * @return
                     */
                    bool begin() {
                        webServer.on("/", [this]() {
                            webServer.send(200, "text/plain", getWelcomeMessage());
                        });

                        webServer.begin(port);

                        return setErrorMessage("");
                    }

                    /**
                     * Handle requests
                     */
                    void handle() {
                        webServer.handleClient();
                    }

                    /**
                     *
                     * @return
                     */
                    String getWelcomeMessage() {
                        return String("List of endpoints:\n") + welcomeMessage;
                    }

                protected:
                    uint16_t port;
                    String welcomeMessage;
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_APISERVER_H
