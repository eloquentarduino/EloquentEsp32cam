//
// Created by Simone on 17/12/22.
//

#ifndef ELOQUENTESP32CAM_YCBCRHTTP_H
#define ELOQUENTESP32CAM_YCBCRHTTP_H


#include <WebServer.h>
#include "../Cam.h"
#include "../JpegDecoder.h"
#include "./features/DisplaysJpegFeed.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            class YCbCrHTTP : public HasErrorMessage {
            public:
                WebServer server;
                Cam *cam;
                JpegDecoder *decoder;

                /**
                 *
                 * @param cam
                 * @param decoder
                 */
                YCbCrHTTP(Cam& camera, JpegDecoder& jpegDecoder) :
                    cam(&camera),
                    decoder(&jpegDecoder),
                    jpeg(server, camera) {

                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    if (!jpeg.probeCamera())
                        return setErrorMessage(jpeg.getErrorMessage());

                    jpeg.onRequest([this]() {
                        decoder->decode(*cam);
                    });

                    server.on("/", [this]() {
                        return this->onIndex();
                    });

                    server.on("/ycbcr", [this]() {
                        this->onYCbCr();
                    });

                    server.begin();

                    return setErrorMessage("");
                }

                /**
                 *
                 */
                void handle() {
                    server.handleClient();
                }

                /**
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    return
                            String(F("YCbCr Feed webpage available at http://"))
                            + cam->getAddress()
                            + ". JPEG feed available at /feed. YCbCr available at /ycbcr";
                }

            protected:
                Features::DisplaysJpegFeed jpeg;

                /**
                 *
                 * @return
                 */
                void onYCbCr() {
                    if (!decoder->isOk()) {
                        server.send(500, "text/plain", decoder->getErrorMessage());
                        return;
                    }

                    auto client = server.client();
                    const uint16_t lenY = decoder->luma.length;
                    const uint16_t lenCb = decoder->cb.length;
                    const uint16_t lenCr = decoder->cr.length;

                    client.println(F("HTTP/1.1 200 OK"));
                    client.println(F("Content-Type: application/octet-stream"));
                    client.println(F("Connection: close"));
                    client.print(F("Content-Length: "));
                    client.println(lenY + lenCb + lenCr);
                    client.print(F("X-Width: "));
                    client.println(decoder->getWidth() / 8);
                    client.print(F("X-Height: "));
                    client.println(decoder->getHeight() / 8);
                    client.print(F("X-Luma: "));
                    client.println(lenY);
                    client.print(F("X-Cb: "));
                    client.println(lenCb);
                    client.print(F("X-Cr: "));
                    client.println(lenCr);
                    client.println();
                    client.write(decoder->luma.pixels, lenY);
                    client.write(decoder->cb.pixels, lenCb);
                    client.write(decoder->cr.pixels, lenCr);
                }

                /**
                 *
                 * @return
                 */
                bool onIndex() {
                    Features::Resources resources(server);

                    jpeg.css();
                    resources.css(".flex {display: flex; } .flex > div {flex: 1;}");

                    server.sendContent(F(R"===(
                        <div id="app">
                            <div id="canvas">
                                <img id="jpeg" />
                                <div class="flex">
                                    <div><canvas id="y"></canvas><h4>Y</h4></div>
                                    <div><canvas id="cb"></canvas><h4>Cb</h4></div>
                                    <div><canvas id="cr"></canvas><h4>Cr</h4></div>
                                </div>
                            </div>
                        </div>
                    )==="));

                    jpeg.js();
                    resources.onDOMContentLoaded(F(R"===(
                        const app = document.getElementById("app")
                        const y = document.getElementById("y")
                        const cb = document.getElementById("cb")
                        const cr = document.getElementById("cr")

                        let width
                        let height
                        let chunk

                        function toImage(arr) {
                            const rgb = new Uint8ClampedArray(arr.length * 4)

                            for (let i = 0; i < arr.length; i++) {
                                const p = arr[i]
                                rgb[4 * i + 0] = p
                                rgb[4 * i + 1] = p
                                rgb[4 * i + 2] = p
                                rgb[4 * i + 3] = 255
                            }

                            return new ImageData(rgb, width, height)
                        }

                        function drawImage(canvas, image) {
                            const w = canvas.parentElement.clientWidth
                            const scale = w / width

                            canvas.style.transform = `scale(${scale}, ${scale})`
                            canvas.getContext('2d').putImageData(image, 0, 0)
                        }

                        function drawYCbCr() {
                            try {
                                fetch("/ycbcr")
                                    .then(res => {
                                        if (parseInt(res.headers.get("X-Width")) > 0) {
                                            width = parseInt(res.headers.get("X-Width"))
                                            height = parseInt(res.headers.get("X-Height"))
                                            chunk = width * height

                                            app.style.width = `${width * 8}px`
                                            ;[].slice.call(document.querySelectorAll(".flex > div")).forEach(div => {div.style.width = `${width * 8 / 3}px`})

                                            return res.arrayBuffer()
                                        }
                                    })
                                    .then(buffer => {
                                        if (!buffer) {
                                            console.error("No data received")
                                            return drawYCbCr();
                                        }

                                        const arr = [...new Uint8ClampedArray(buffer)]
                                        const l = toImage(arr.slice(0, chunk))
                                        const b = toImage(arr.slice(chunk, chunk * 2))
                                        const r = toImage(arr.slice(chunk * 2))

                                        drawImage(y, l)
                                        drawImage(cb, b)
                                        drawImage(cr, r)
                                        drawYCbCr();
                                    })
                            } catch (e) { drawYCbCr(); }
                        }

                        drawYCbCr()
                    )==="));

                    server.sendContent("");
                    return true;
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_YCBCRHTTP_H
