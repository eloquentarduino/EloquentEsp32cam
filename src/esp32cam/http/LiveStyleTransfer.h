//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_LIVE_FEED_H
#define ELOQUENTESP32CAM_LIVE_FEED_H


#include <WiFi.h>
#include <WebServer.h>
#include "../Cam.h"
#include "../../traits/HasErrorMessage.h"
#include "./HttpServer.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             * ESP32 camera streaming web server with Style Transfer
             */
            class LiveStyleTransfer :
                    public HasErrorMessage {

            public:
                uint16_t port;

                /**
                 *
                 * @param port
                 */
                LiveStyleTransfer(Cam& cam, uint16_t httpPort = 80) :
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
                        httpd_resp_sendstr(req, R"===(
<style>
.flex {display: flex }
</style>
<div id="app">
    <div>
        <select id="styles">
            <option selected value="vangogh">Van Gogh</value>
            <option value="picasso">Picasso</value>
            <option value="klimt">Klimt</value>
        </select>
        <div hidden>
            <img id="vangogh" width="1280" height="1014" crossorigin="anonymous" src="https://eloquentarduino.com/mastering-the-esp32-camera/book-assets/styles/vangogh.jpeg" />
            <img id="picasso" width="1600" height="900" crossorigin="anonymous" src="https://eloquentarduino.com/mastering-the-esp32-camera/book-assets/styles/picasso.jpeg" />
            <img id="klimt" width="800" height="800" crossorigin="anonymous" src="https://eloquentarduino.com/mastering-the-esp32-camera/book-assets/styles/klimt.jpeg" />
        </div>
    </div>
    <div class="flex">
        <img id="feed" crossorigin="anonymous" />
        <canvas id="canvas"></canvas>
    </div>
</div>
<script src="https://cdn.jsdelivr.net/npm/@magenta/image@^0.2.1"></script>
<script>
const styles = document.getElementById('styles')
const contentImg = document.getElementById('feed');
const stylizedCanvas = document.getElementById('canvas')
const transfer = new mi.ArbitraryStyleTransferNetwork()

contentImg.onload = () => {
    setTimeout(() => {
        contentImg.setAttribute('width', contentImg.clientWidth)
        contentImg.setAttribute('height', contentImg.clientHeight)
    }, 500)
}
contentImg.src = 'http://' + window.location.host + ':' + (parseInt(window.location.port || 80) + 1);


function applyStyle() {
    const styleImg = document.getElementById(styles.value);

    console.log('apply style', styles.value)

    transfer.stylize(contentImg, styleImg).then((imageData) => {
        stylizedCanvas.getContext('2d').putImageData(imageData, 0, 0);
        applyStyle()
    });
}

transfer.initialize().then(() => {
    applyStyle();
})
</script>
)===");

                        return ESP_OK;
                    });

                    if (!_indexServer.begin())
                        return setErrorMessage("Cannot create HTTP main server");

                    _mjpegServer.get("/", [](httpd_req_t *req) {
                        esp_err_t res = ESP_OK;
                        char *part[64];
                        Cam * cam = (Cam * ) req->user_ctx;

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

                            if (httpd_resp_send_chunk(req, (const char *) cam->frame->buf,
                                                      cam->getFileSize()) != ESP_OK)
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
                            String(F("Live Style Transfer available at http://"))
                            + ip
                            + (port != 80 ? String(':') + port : "");
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
