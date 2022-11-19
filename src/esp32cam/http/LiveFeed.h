//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_LIVE_FEED_H
#define ELOQUENTESP32CAM_LIVE_FEED_H


#include "../Cam.h"
#include "../../traits/HasErrorMessage.h"
#include "./HttpServer.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             * ESP32 camera streaming web server
             */
            class LiveFeed :
                    public HasErrorMessage {

            public:
                uint16_t port;

                /**
                 *
                 * @param port
                 */
                LiveFeed(Cam& cam, uint16_t httpPort = 80) :
                _cam (&cam),
                _indexServer(httpPort),
                _mjpegServer(httpPort + 1),
                port(httpPort) {

                    setErrorMessage("");
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    _indexServer.get("/", [](httpd_req_t *req) {
                        httpd_resp_sendstr(
                                req,
                                "<div id=\"app\"><img id=\"feed\" /></div>"
                                "<script>document.getElementById(\"feed\").src = 'http://' + window.location.host + ':' + (parseInt(window.location.port || 80) + 1);</script>"
                        );

                        return ESP_OK;
                    });

                    if (!_indexServer.begin())
                        return setErrorMessage("Cannot create HTTP main server");

                    _mjpegServer.get("/", [](httpd_req_t *req) {
                        esp_err_t res = ESP_OK;
                        char *part[64];
                        Cam * cam = (Cam *) req->user_ctx;

                        if (httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=frame") != ESP_OK)
                            return ESP_FAIL;

                        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

                        while (true) {
                            size_t contentTypeHeaderLength;

                            if (!cam->capture()) {
                                delay(1);
                                continue;
                            }

                            contentTypeHeaderLength = snprintf(
                                    (char *) part, 64,
                                    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                                    cam->getFileSize()
                            );

                            if (httpd_resp_send_chunk(req, (const char *) part, contentTypeHeaderLength) != ESP_OK)
                                return ESP_FAIL;

                            if (httpd_resp_send_chunk(req, (const char *) cam->frame->buf, cam->getFileSize()) != ESP_OK)
                                return ESP_FAIL;

                            res = httpd_resp_send_chunk(req, "\r\n--frame\r\n", 12);
                        }

                        return ESP_OK;
                    }, _cam);

                    if (!_mjpegServer.begin())
                        return setErrorMessage("Cannot create HTTP MJPEG server");

                    return true;
                }

                /**
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    String ip = this->_cam->getIP();

                    return
                            String(F("Live Feed webpage available at http://"))
                            + ip
                            + (port != 80 ? String(':') + port : "")
                            + ". MJPEG stream available at http://"
                            + ip
                            + ':'
                            + (port + 1);
                }

            protected:
                Cam *_cam;
                HttpServer<1> _indexServer;
                HttpServer<1> _mjpegServer;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_LIVE_FEED_H
