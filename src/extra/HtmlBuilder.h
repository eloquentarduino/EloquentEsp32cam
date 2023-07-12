#pragma once

#include <WiFi.h>
#include <WebServer.h>


namespace Eloquent {
    namespace Extra {
        /**
         * Utility class for improved HTML generation
         */
        class HtmlBuilder {
            public:

                /**
                 * Constructor
                 */
                HtmlBuilder(WebServer *server) :
                    _server(server) {

                    }

                /**
                 * Send response of unknown length
                 *
                 * @tparam Callback
                 * @param callback
                 */
                template<typename Callback>
                void open(Callback callback, const char* contentType = "text/html", uint16_t statusCode = 200) {
                    _server->setContentLength(CONTENT_LENGTH_UNKNOWN);
                    _server->send(statusCode, contentType, "");
                    callback();
                    _server->sendContent("");
                }

                /**
                 *
                 * @param path
                 * @return
                 */
                void image(String path) {
                    IPAddress ipAddr = WiFi.localIP();
                    String ip = String(ipAddr[0]) + '.' + ipAddr[1] + '.' + ipAddr[2] + '.' + ipAddr[3];

                    if (path[0] == '/' or path[0] == ':') {
                        path = String("http://") + ip + path;
                    }

                    _server->sendContent("<img src=\"");
                    _server->sendContent(path);
                    _server->sendContent("\" />");
                }

                /**
                 * Declare JS env variable
                 */
                template<typename T>
                void jsEnv(String key, T value, String converter = "") {
                    _server->sendContent(F("<script>window.ENV = window.ENV || {}; window.ENV."));
                    _server->sendContent(key);
                    _server->sendContent(F(" = `"));
                    _server->sendContent(String(value));
                    _server->sendContent(F('`'));

                    if (converter != "")
                        _server->sendContent(converter);

                    _server->sendContent(F("</script>"));
                }

                /**
                 * Create svelte app container
                 * (also load TailwindCSS)
                 */
                void svelteApp() {
                    _server->sendContent(F("<div id=\"app\"></div>"));
                    _server->sendContent(F("<script src=\"/app.js\"></script>"));
                    _server->sendContent(F("<script src=\"https://cdn.tailwindcss.com\"></script>"));
                }

                /**
                 * Get absolute URL for path
                 */
                String getAbsoluteURL(String path) {
                    return getIP() + path;
                }

            protected:
                WebServer *_server;

                /**
                 * Get IP address as string
                 *
                 * @return
                 */
                String getIP() {
                    IPAddress ip = WiFi.localIP();

                    return String(ip[0]) + '.' + ip[1] + '.' + ip[2] + '.' + ip[3];
                }
        };
    }
}