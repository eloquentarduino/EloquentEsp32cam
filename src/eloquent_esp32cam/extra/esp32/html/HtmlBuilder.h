#ifndef ELOQUENT_EXTRA_ESP32_HTML_HTMLBUILDER
#define ELOQUENT_EXTRA_ESP32_HTML_HTMLBUILDER

#include <WebServer.h>

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Html {
                /**
                 * Convenience class for HTML generation
                 */
                class HtmlBuilder {
                    public:
                        WebServer *webServer;

                        /**
                         * Constructor
                         */
                        HtmlBuilder(WebServer& server) :
                            webServer(&server) {

                            }

                        /**
                         * Render Svelte.js SPA
                         */
                        void svelteSPA() {
                            const char app[] PROGMEM = R"===(
                                <div id="app"></div>
                                <script src="/app.js"></script>
                                <script src="https://cdn.tailwindcss.com"></script>
                            )===";

                            webServer->send(200, "text/html", app);
                        }
                };
            }
        }
    }
}

#endif