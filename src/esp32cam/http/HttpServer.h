//
// Created by Simone on 13/11/22.
//

#ifndef ELOQUENTESP32CAM_HTTPSERVER_H
#define ELOQUENTESP32CAM_HTTPSERVER_H

#include <esp_http_server.h>

namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             * Generic HTTP server utility class
             */
             template<uint8_t numUris>
            class HttpServer {
            public:
                httpd_config_t config;
                httpd_handle_t httpd;
                httpd_uri_t uris[numUris];

                /**
                 *
                 * @param port
                 */
                HttpServer(uint16_t port) :
                _port(port),
                _count(0) {
                    config = HTTPD_DEFAULT_CONFIG();
                    listenOn(port);
                }

                /**
                 * Set listening port
                 *
                 * @param port
                 */
                void listenOn(uint16_t port) {
                    config.server_port = port;
                    config.ctrl_port = port;
                }

                /**
                 *
                 * @param uri
                 * @param handler
                 * @return
                 */
                template<typename Handler>
                bool get(const char *uri, Handler handler) {
                    if (_count >= numUris)
                        return false;

                    uris[_count].uri = uri;
                    uris[_count].method = HTTP_GET;
                    uris[_count].handler = handler;
                    uris[_count].user_ctx = NULL;

                    _count += 1;

                    return true;
                }

                /**
                 *
                 * @param uri
                 * @param handler
                 * @param ctx
                 * @return
                 */
                template<typename Handler, typename Context>
                bool get(const char *uri, Handler handler, Context *ctx) {
                    if (_count >= numUris)
                        return false;

                    uris[_count].uri = uri;
                    uris[_count].method = HTTP_GET;
                    uris[_count].handler = handler;
                    uris[_count].user_ctx = (void *) ctx;

                    _count += 1;

                    return true;
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    if (httpd_start(&httpd, &config) != ESP_OK)
                        return false;

                    for (uint8_t i = 0; i < _count; i++)
                        httpd_register_uri_handler(httpd, &uris[i]);

                    return true;
                }

            protected:
                uint16_t _port;
                uint8_t _count;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_HTTPSERVER_H
