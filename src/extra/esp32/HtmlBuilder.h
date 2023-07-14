#pragma once

#include <WiFi.h>
#include <WebServer.h>


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            /**
             * Utility class for improved HTML generation
             */
            class HtmlBuilder {
                public:
                    WebServer *webServer;

                    /**
                     * Constructor
                     */
                    HtmlBuilder(WebServer *server) :
                        webServer(server) {

                        }

                    /**
                     * Send response of unknown length
                     *
                     * @tparam Callback
                     * @param callback
                     */
                    template<typename Callback>
                    void open(Callback callback, const char* contentType = "text/html", uint16_t statusCode = 200) {
                        webServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
                        webServer->send(statusCode, contentType, "");
                        callback();
                        webServer->sendContent("");
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

                        webServer->sendContent("<img src=\"");
                        webServer->sendContent(path);
                        webServer->sendContent("\" />");
                    }

                    /**
                     * Declare JS env variable
                     */
                    template<typename T>
                    void jsEnv(String key, T value, String converter = "") {
                        webServer->sendContent(F("<script>window.ENV = window.ENV || {}; window.ENV."));
                        webServer->sendContent(key);
                        webServer->sendContent(F(" = `"));
                        webServer->sendContent(String(value));
                        webServer->sendContent(F('`'));

                        if (converter != "")
                            webServer->sendContent(converter);

                        webServer->sendContent(F("</script>"));
                    }

                    /**
                     * Create svelte app container
                     * (also load TailwindCSS)
                     */
                    void svelteApp() {
                        webServer->sendContent(F("<div id=\"app\"></div>"));
                        webServer->sendContent(F("<script src=\"/app.js\"></script>"));
                        webServer->sendContent(F("<script src=\"https://cdn.tailwindcss.com\"></script>"));
                    }

                    /**
                     * Get absolute URL for path
                     */
                    String getAbsoluteURL(String path) {
                        return getIP() + path;
                    }

                protected:
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
}