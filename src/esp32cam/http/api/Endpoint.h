//
// Created by Simone on 17/02/23.
//

#ifndef ELOQUENTESP32CAM_ENDPOINT_H
#define ELOQUENTESP32CAM_ENDPOINT_H

#include <WebServer.h>


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Api {
                class Endpoint {
                public:

                    /**
                     * Handle request
                     */
                    virtual void handle(WebServer& webServer) = 0;

                    /**
                     * Get endpoint route
                     *
                     * @return
                     */
                    virtual String endpoint() = 0;
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_ENDPOINT_H
