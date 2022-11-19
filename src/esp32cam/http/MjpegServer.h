//
// Created by Simone on 17/11/22.
//

#ifndef ELOQUENTESP32CAM_MJPEGSERVER_H
#define ELOQUENTESP32CAM_MJPEGSERVER_H


#include "../Cam.h"
#include "../motion/Detector.h"
#include "./HttpServer.h"
#include "../../traits/HasErrorMessage.h"



namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             * Helper structure
             */
            struct MjpegBag {
                Cam* cam;
                Motion::Detector* detector;
            };

            /**
             * HTTP server to server MJPEG live feed
             */
            class MjpegServer : public HasErrorMessage {
            public:

                /**
                 * @param cam
                 * @param port
                 */
                MjpegServer(Cam& cam, uint16_t port) :
                    _server(port) {
                    _bag.cam = &cam;
                    _bag.detector = NULL;
                }

                /**
                 *
                 * @param detector
                 */
                inline void motion(Motion::Detector& detector) {
                    _bag.detector = &detector;
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    _server.get("/", [](httpd_req_t *req) {
                        esp_err_t res = ESP_OK;
                        char *part[64];
                        MjpegBag *bag = (MjpegBag*) req->user_ctx;
                        Cam *cam = bag->cam;
                        Motion::Detector *detector = bag->detector;

                        if (httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=frame") != ESP_OK)
                            return ESP_FAIL;

                        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

                        while (true) {
                            size_t contentTypeHeaderLength;

                            if (!cam->capture()) {
                                yield();
                                continue;
                            }

                            contentTypeHeaderLength = snprintf(
                                    (char *) part, 64,
                                    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                                    cam->getFileSize()
                            );

                            if (httpd_resp_send_chunk(req, (const char *) part, contentTypeHeaderLength) != ESP_OK)
                                return ESP_FAIL;

                            if (httpd_resp_send_chunk(req, (const char *) cam->frame->buf,
                                                      cam->getFileSize()) != ESP_OK)
                                return ESP_FAIL;

                            res = httpd_resp_send_chunk(req, "\r\n--frame\r\n", 12);
                        }

                        return ESP_OK;
                    }, &_bag);

                    if (!_server.begin())
                        return setErrorMessage("Cannot create HTTP MJPEG server");

                    return true;
                }

            protected:
                HttpServer<1> _server;
                MjpegBag _bag;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_MJPEGSERVER_H
