//
// Created by Simone on 09/02/23.
//

#ifndef ELOQUENTESP32CAM_HTML_H
#define ELOQUENTESP32CAM_HTML_H

#include <WebServer.h>

namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Features {
                /**
                 * Create HTML pages with ease
                 */
                class HTML {
                public:
                    WebServer webServer;

                    /**
                     * Write HTML to webpage
                     *
                     * @tparam Html
                     * @param html
                     */
                    template<typename Html>
                    void html(Html html) {
                        webServer.sendContent(html);
                    }

                    /**
                     * Write text to webpage
                     *
                     * @tparam Text
                     * @param text
                     */
                    template<typename Text>
                    void text(Text text) {
                        webServer.sendContent(text);
                    }

                    /**
                     *
                     * @param c
                     */
                    void text(char c) {
                        text(String(c));
                    }

                    /**
                     * Add inline CSS styles
                     *
                     * @param css
                     */
                    template<typename Css>
                    void inlineCSS(Css css) {
                        webServer.sendContent(F("<style>"));
                        webServer.sendContent(css);
                        webServer.sendContent(F("</style>"));
                    }

                    /**
                     * Add inline JS code
                     *
                     * @param js
                     */
                    template<typename Js>
                    void inlineJs(Js js) {
                        webServer.sendContent(F("<script>"));
                        webServer.sendContent(js);
                        webServer.sendContent(F("</script>"));
                    }

                    /**
                     * Add remote JS file
                     *
                     * @param src
                     * @param attributes
                     */
                    void remoteJs(const char *src, const char *attributes = "") {
                        webServer.sendContent(F("<script src=\""));
                        webServer.sendContent(src);
                        webServer.sendContent(F("\" "));
                        webServer.sendContent(attributes);
                        webServer.sendContent(F("></script>"));
                    }

                    /**
                     *
                     */
                    void alpinejs() {
                        remoteJs("https://cdn.jsdelivr.net/npm/alpinejs@3.x.x/dist/cdn.min.js", "defer");
                    }

                    /**
                     *
                     */
                    void tailwindcss() {
                        remoteJs("https://cdn.tailwindcss.com", "defer");
                    }

                    /**
                     * Declare js variable
                     *
                     * @param name
                     * @param value
                     */
                    template<typename T>
                    void declareJsVariable(const char *name, T value) {
                        webServer.sendContent(F("<script>window.APP = window.APP || {}; window.APP."));
                        webServer.sendContent(name);
                        webServer.sendContent(F(" = "));
                        webServer.sendContent(String(value));
                        webServer.sendContent(F("</script>"));
                    }

                    /**
                     * Add js code to be called on DOMContentLoaded event
                     *
                     * @tparam Code
                     * @param code
                     */
                    template<typename Code>
                    void onDOMContentLoaded(Code code) {
                        webServer.sendContent(F("<script>window.addEventListener(\"DOMContentLoaded\", () => setTimeout(() => {\n"));
                        webServer.sendContent(code);
                        webServer.sendContent(F("\n}, 100))</script>"));
                    }

                    /**
                     * Render arbitrary component
                     *
                     * @tparam Component
                     * @param component
                     */
                    template<typename Component>
                    void component(Component component) {
                        component.renderTo(*this);
                    }

                    /**
                     * Send response to browser
                     */
                    void end() {
                        webServer.sendContent("");
                    }

                    /**
                     * Write binary data
                     *
                     * @param buffer
                     * @param length
                     * @param contentType
                     */
                    void write(uint8_t *buffer, size_t length, const char *contentType = "application/octet-stream") {
                        WiFiClient client = webServer.client();

                        client.println(F("HTTP/1.1 200 OK"));
                        client.print(F("Content-Type: "));
                        client.println(contentType);
                        client.println(F("Connection: close"));
                        client.print(F("Content-Length: "));
                        client.println(length);
                        client.println();

                        client.write(buffer, length);
                    }
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_HTML_H
