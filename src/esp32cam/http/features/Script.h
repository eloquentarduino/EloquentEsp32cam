//
// Created by Simone on 24/12/22.
//

#ifndef ELOQUENTESP32CAM_HTTP_JS_SCRIPT_H
#define ELOQUENTESP32CAM_HTTP_JS_SCRIPT_H

#include <WebServer.h>

namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Features {

                /**
                 * Add JS code to webpage
                 */
                class Script {
                public:

                    /**
                     *
                     * @param server
                     */
                    Script(WebServer& server) :
                        _server(&server) {

                    }

                    /**
                     *
                     * @param server
                     */
                    Script(WebServer *server) :
                        _server(server) {

                    }

                    /**
                     *
                     * @tparam Code
                     * @param server
                     * @param code
                     */
                    template<typename Code>
                    Script(WebServer& server, Code code) :
                        Script(server) {

                        add(code);
                    }

                    /**
                     * Add js code
                     * @tparam Code
                     * @param code
                     */
                    template<typename Code>
                    void add(Code code) {
                        _server->sendContent(F("<script>"));
                        _server->sendContent(code);
                        _server->sendContent(F("</script>"));
                    }

                    /**
                     * Add function to be called on DOMContentLoaded event
                     *
                     * @param functionName
                     * @param delay
                     */
                    void onDOMContentLoaded(const char *functionName, uint16_t delay) {
                        _server->sendContent(F("<script>addEventListener(\"DOMContentLoaded\", () => setTimeout("));
                        _server->sendContent(functionName);
                        _server->sendContent(F(", "));
                        _server->sendContent(String(delay));
                        _server->sendContent(F("))</script>"));
                    }

                    /**
                     * Add function to be called on DOMContentLoaded event
                     *
                     * @tparam Code
                     * @param code
                     */
                    template<typename Code>
                    Script& onDOMContentLoaded(Code code) {
                        _server->sendContent(F("<script>addEventListener(\"DOMContentLoaded\", () => setTimeout(() => {\n"));
                        _server->sendContent(code);
                        _server->sendContent(F("\n}, 100))</script>"));

                        return *this;
                    }

                protected:
                    WebServer *_server;
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_HTTP_JS_SCRIPT_H
