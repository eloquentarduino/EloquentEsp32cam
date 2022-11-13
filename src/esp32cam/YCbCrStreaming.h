//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_YCBCRSTREAMING_H
#define ELOQUENTESP32CAM_YCBCRSTREAMING_H


#include "Cam.h"
#include "../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        /**
         * ESP32 camera streaming web server for Y, Cb, Cr components
         */
        class YCbCrStreaming : public HasErrorMessage {

        public:
            uint16_t port;

            /**
             *
             * @param port
             */
            YCbCrStreaming(Cam& cam, uint16_t startPort = 80) :
                    _cam(&cam),
                    port(startPort) {

                setErrorMessage("");
            }

            /**
            *
            * @return
            */
            bool begin() {
                bool ok;

                ok = _cam->addHttpServer(port, [this](httpd_handle_t *httpd) {
                    httpd_uri_t index_uri = {
                            .uri       = "/",
                            .method    = HTTP_GET,
                            .handler   = [](httpd_req_t *req) {
                                httpd_resp_sendstr(req, "<script src=\"https://eloquentarduino.com/esp32cam-book/js/ycbcr_server.js\"></script>");
                                return ESP_OK;
                            },
                            .user_ctx = NULL
                    };

                    httpd_uri_t stream_uri = {
                            .uri       = "/stream",
                            .method    = HTTP_GET,
                            .handler   = [](httpd_req_t *req) {
                                Cam *cam = (Cam *) req->user_ctx;

                                if (!cam->capture()) {
                                    httpd_resp_send(req, "!C", 2);
                                    return ESP_FAIL;
                                }

                                if (!cam->decode()) {
                                    httpd_resp_send(req, "!D", 2);
                                    return ESP_FAIL;
                                }

                                if (!cam->Luma.len) {
                                    httpd_resp_send(req, "!L", 2);
                                    return ESP_FAIL;
                                }

                                const uint16_t lumaLength = cam->Luma.len;
                                const uint16_t chromaLength = cam->Cb.len;
                                char contentLengthBuf[8];
                                char lumaLengthBuf[8];
                                char chromaLengthBuf[8];
                                char widthBuf[8];
                                char heightBuf[8];

                                httpd_resp_set_hdr(req, "Content-Type", "application/octet-stream");
                                sprintf(contentLengthBuf, "%d", lumaLength + 2 * chromaLength);
                                httpd_resp_set_hdr(req, "Content-Length", contentLengthBuf);
                                sprintf(lumaLengthBuf, "%d", lumaLength);
                                httpd_resp_set_hdr(req, "X-Luma-Length", lumaLengthBuf);
                                sprintf(chromaLengthBuf, "%d", chromaLength);
                                httpd_resp_set_hdr(req, "X-Chroma-Length", chromaLengthBuf);
                                sprintf(widthBuf, "%d", cam->getJpegWidth());
                                httpd_resp_set_hdr(req, "X-Jpeg-Width", widthBuf);
                                sprintf(heightBuf, "%d", cam->getJpegHeight());
                                httpd_resp_set_hdr(req, "X-Jpeg-Height", heightBuf);

                                httpd_resp_send_chunk(req, (const char *) cam->Luma.buf, lumaLength);
                                httpd_resp_send_chunk(req, (const char *) cam->Cb.buf, chromaLength);
                                httpd_resp_send_chunk(req, (const char *) cam->Cr.buf, chromaLength);
                                httpd_resp_send_chunk(req, "", 0);

                                return ESP_OK;
                            },
                            .user_ctx  = (void *)_cam,
                    };


                    httpd_register_uri_handler(*httpd, &index_uri);
                    httpd_register_uri_handler(*httpd, &stream_uri);
                });

                if (!ok)
                    return setErrorMessage("Cannot start HTTP YCbCr streaming server");

                return true;
            }


            /**
             *
             * @return
             */
            String getWelcomeMessage() {
                String ip = this->_cam->getIP();

                if (port != 80) {
                    ip += ':';
                    ip += port;
                }

                return
                        String(F("YCbCr Streaming available at http://"))
                        + ip;
            }

        protected:
            Cam *_cam;
        };
    }
}

#endif //ELOQUENTESP32CAM_YCBCRSTREAMING_H
