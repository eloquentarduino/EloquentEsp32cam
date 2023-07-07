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

            protected:
                WebServer *_server;
        };
    }
}