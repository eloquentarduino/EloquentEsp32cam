//
// Created by Simone on 31/12/22.
//

#ifndef ELOQUENTESP32CAM_ASSETS_H
#define ELOQUENTESP32CAM_ASSETS_H

#include <WebServer.h>

namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Features {

                /**
                 * Add assets (css, js) to webpage
                 */
                class Resources {
                public:
                    WebServer *server;

                    /**
                     *
                     * @param webServer
                     */
                    Resources(WebServer& webServer) :
                        server(&webServer) {

                    }

                    /**
                     *
                     * @param webServer
                     */
                    Resources(WebServer *webServer) :
                        server(webServer) {

                    }

                    /**
                     * Add css
                     *
                     * @tparam Code
                     * @param code
                     */
                    template<typename Code>
                    void css(Code code) {
                        server->sendContent(F("<style>"));
                        server->sendContent(code);
                        server->sendContent(F("</style>"));
                    }

                    /**
                     * Add js
                     *
                     * @tparam Code
                     * @param code
                     */
                    template<typename Code>
                    void js(Code code) {
                        server->sendContent(F("<script>"));
                        server->sendContent(code);
                        server->sendContent(F("</script>"));
                    }

                    /**
                     * Declare js variable
                     *
                     * @param name
                     * @param value
                     */
                    template<typename T>
                    void variable(const char *name, T value) {
                        server->sendContent(F("<script>window.APP = window.APP || {}; window.APP."));
                        server->sendContent(name);
                        server->sendContent(F(" = "));
                        server->sendContent(String(value));
                        server->sendContent(F("</script>"));
                    }

                    /**
                     * Add js code to be called on DOMContentLoaded event
                     *
                     * @tparam Code
                     * @param code
                     */
                    template<typename Code>
                    void onDOMContentLoaded(Code code) {
                        server->sendContent(F("<script>addEventListener(\"DOMContentLoaded\", () => setTimeout(() => {\n"));
                        server->sendContent(code);
                        server->sendContent(F("\n}, 100))</script>"));
                    }
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_ASSETS_H
