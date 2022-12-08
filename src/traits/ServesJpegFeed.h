//
// Created by Simone on 07/12/22.
//

#ifndef ELOQUENTESP32CAM_SERVESJPEGFEED_H
#define ELOQUENTESP32CAM_SERVESJPEGFEED_H

#include <WebServer.h>
#include "../esp32cam/Cam.h"
#include "../esp32cam/JpegDecoder.h"
#include "./HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {

        /**
         * Trait for HTTP servers that send JPEG images
         */
        class ServesJpegFeed : public HasErrorMessage {
        public:
            WebServer server;
            Cam *cam;
            JpegDecoder *jpegDecoder;

            /**
             *
             * @param camera
             * @param decoder
             * @param port
             */
            ServesJpegFeed(
                    Cam& camera,
                    JpegDecoder& decoder,
                    uint16_t port = 80) :
                    cam(&camera),
                    jpegDecoder(&decoder),
                    server(port) {

            }

        protected:

            /**
             * Abort with 500 error
             *
             * @param message
             * @return
             */
            bool abort(String message) {
                server.send(500, "text/plain", message);

                return false;
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
            void addJpegFeedScript() {
                server.sendContent(F("<script>var imgWidth = "));
                server.sendContent(String(cam->getWidth()));
                server.sendContent(F("; var imgHeight = "));
                server.sendContent(String(cam->getHeight()));
                server.sendContent(F("</script>"));
                server.sendContent(F(R"===(
                    <script>
                    const feed = document.getElementById("feed")
                    function drawFeed() {
                        fetch("/feed")
                            .then(res => res.arrayBuffer())
                            .then(jpeg => {
                                const blob = new Blob([jpeg], { type: "image/jpeg" });
                                feed.src = (window.URL || window.webkitURL).createObjectURL(blob)
                                feed.style.width = `${imgWidth}px`
                                feed.style.height = `${imgHeight}px`
                                drawFeed()
                            })
                    }
                    setTimeout(() => drawFeed(), 2000)
                    </script>
                )==="));
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_SERVESJPEGFEED_H
