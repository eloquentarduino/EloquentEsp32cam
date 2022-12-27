//
// Created by Simone on 20/12/22.
//

#ifndef ELOQUENTESP32CAM_SERVESHTML_H
#define ELOQUENTESP32CAM_SERVESHTML_H

#include <WebServer.h>
#include "../../esp32cam/http/features/Script.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             * Base class for WebServer-related clasess
             */
            class ServesHTML {
            public:
                WebServer server;
                uint16_t httpPort;

                /**
                 *
                 * @param port
                 */
                ServesHTML(uint16_t port = 80) :
                    httpPort(port),
                    server(port),
                    js(&server) {

                }

                /**
                 *
                 * @param port
                 */
                void listenOn(uint16_t port) {
                    httpPort = port;
                }

                /**
                 * Handle client connection
                 */
                virtual void handle() {
                    server.handleClient();
                }

            protected:
                Features::Script js;


                /**
                 * Abort with 500 error
                 *
                 * @param message
                 * @return
                 */
                virtual bool abort(String message) {
                    server.send(500, "text/plain", message);

                    return false;
                }

                /**
                 * Send contents to client
                 */
                inline void flush() {
                    server.sendContent("");
                }

                /**
                 * Mark page as new
                 */
                inline void refresh() {
                    _inited = false;
                }

                /**
                 * Add inline CSS to page
                 *
                 * @tparam CSS
                 * @param css
                 */
                template<typename CSS>
                void addCSS(CSS css) {
                    server.sendContent(F("<style>"));
                    server.sendContent(css);
                    server.sendContent(F("</style>"));
                }

                /**
                 * Add inline HTML to page
                 *
                 * @tparam HTML
                 * @param html
                 */
                template<typename HTML>
                void addHTML(HTML html) {
                    server.sendContent(html);
                }

                /**
                 * Add inline JS to page
                 *
                 * @tparam JS
                 * @param js
                 */
                template<typename JS>
                void addJS(JS js) {
                    server.sendContent(F("<script>"));
                    server.sendContent(js);
                    server.sendContent(F("</script>"));
                }

                /**
                 * Declare global variables and functions
                 */
                void initWebpage() {
                    if (!_inited) {
                        _inited = true;

                        addCSS(R"===(
                            #frame {position: relative; display: inline-block;}
                            #canvas {position: relative; display: inline-block;}
                            .flex {display: flex;}
                            canvas {transform-origin: 0 0;}
                        )===");

                        addJS(R"===(
                            window.APP = window.APP || {$dom: {}, constants: {}, listeners: {}}
                        )===");
                    }

                }

                /**
                 * Add function to be called on DOMContentLoaded event
                 *
                 * @param functionName
                 * @param delay
                 */
                void onDOMContentLoaded(const char *functionName, uint16_t delay = 1000) {
                    server.sendContent(F("<script>addEventListener(\"DOMContentLoaded\", () => setTimeout("));
                    server.sendContent(functionName);
                    server.sendContent(F(", "));
                    server.sendContent(String(delay));
                    server.sendContent(F("))</script>"));
                }

                /**
                 *
                 * @tparam T
                 * @param variableName
                 * @param variableValue
                 */
                template<typename T>
                void addJsRawVariable(const char *variableName, T variableValue) {
                    addJsVariableWrappedIn(variableName, variableValue, " ");
                }

                /**
                 *
                 * @tparam T
                 * @param variableName
                 * @param variableValue
                 */
                template<typename T>
                void addJsQuotedVariable(const char *variableName, T variableValue) {
                    addJsVariableWrappedIn(variableName, variableValue, "`");
                }

                /**
                 *
                 * @tparam T
                 * @param variableName
                 * @param variableValue
                 */
                template<typename T>
                void addJsVariableWrappedIn(const char *variableName, T variableValue, const char *wrap) {
                    initWebpage();

                    server.sendContent(F("<script>window.APP.constants."));
                    server.sendContent(variableName);
                    server.sendContent(F(" = "));
                    server.sendContent(wrap);
                    server.sendContent(String(variableValue));
                    server.sendContent(wrap);
                    server.sendContent(F("</script>\n"));
                }

            private:
                bool _inited = false;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SERVESHTML_H
