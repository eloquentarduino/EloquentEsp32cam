//
// Created by Simone on 07/12/22.
//

#ifndef ELOQUENTESP32CAM_SERVESJPEGFEED_H
#define ELOQUENTESP32CAM_SERVESJPEGFEED_H

#include "./ServesHTML.h"
#include "../../esp32cam/Cam.h"
#include "../../esp32cam/JpegDecoder.h"
#include "../HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             * Trait for HTTP servers that send JPEG images
             */
            class ServesJpegFeed : public ServesHTML, public HasErrorMessage {
            public:
                Cam *cam;
                JpegDecoder *jpegDecoder;

                /**
                 *
                 * @param camera
                 * @param decoder
                 * @param port
                 */
                ServesJpegFeed(
                        Cam &camera,
                        JpegDecoder &decoder,
                        uint16_t port = 80) :
                        ServesHTML(80),
                        cam(&camera),
                        jpegDecoder(&decoder) {

                }

            protected:

                /**
                 * Try to capture JPEG frame
                 *
                 * @return
                 */
                bool probeCamera() {
                    if (!cam->capture())
                        return setErrorMessage("Cannot capture frame");

                    if (!jpegDecoder->decode(*cam))
                        return setErrorMessage("Cannot decode JPEG");

                    return setErrorMessage("");
                }


                /**
                 * Capture image
                 * Send 500 on error
                 *
                 * @return
                 */
                bool capture() {
                    if (!cam->capture()) {
                        this->abort("No frame");
                        return setErrorMessage("No frame");
                    }

                    sendJpeg();

                    if (!jpegDecoder->decode(*cam)) {
                        this->abort(jpegDecoder->getErrorMessage());
                        return setErrorMessage(jpegDecoder->getErrorMessage());
                    }

                    return setErrorMessage("");
                }

                /**
                 * Send JPEG
                 *
                 * @return
                 */
                bool sendJpeg() {
                    WiFiClient client = server.client();
                    client.println(F("HTTP/1.1 200 OK"));
                    client.println(F("Content-Type: image/jpeg"));
                    client.println(F("Connection: close"));
                    client.print(F("Content-Length: "));
                    client.println(cam->frame->len);
                    client.println();
                    client.write(cam->frame->buf, cam->frame->len);

                    return true;
                }

                /**
                 *
                 */
                void initJpegFeedScript() {
                    addJsRawVariable("imgWidth", cam->getWidth());
                    addJsRawVariable("imgHeight", cam->getHeight());

                    addJS(F(R"===(
                        function drawFeed() {
                            try {
                                fetch("/feed")
                                    .then(res => res.arrayBuffer())
                                    .then(jpeg => {
                                        const feed = document.getElementById("feed")
                                        const blob = new Blob([jpeg], { type: "image/jpeg" });
                                        feed.src = (window.URL || window.webkitURL).createObjectURL(blob)
                                        feed.style.width = `${window.APP.constants.imgWidth}px`
                                        feed.style.height = `${window.APP.constants.imgHeight}px`
                                        drawFeed()
                                    })
                            } catch (e) { drawFeed() }
                        }
                    )==="));

                    onDOMContentLoaded("drawFeed");
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SERVESJPEGFEED_H
