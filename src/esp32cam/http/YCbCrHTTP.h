//
// Created by Simone on 17/12/22.
//

#ifndef ELOQUENTESP32CAM_YCBCRHTTP_H
#define ELOQUENTESP32CAM_YCBCRHTTP_H


#include "../../traits/http/ServesJpegFeed.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            class YCbCrHTTP : public ServesJpegFeed {
            public:

                /**
                 *
                 * @param cam
                 * @param decoder
                 * @param httpPort
                 */
                YCbCrHTTP(Cam& cam, JpegDecoder& decoder, uint16_t httpPort = 80) :
                    ServesJpegFeed(cam, decoder, httpPort) {

                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    server.on("/", [this]() {
                        return this->onIndex();
                    });

                    server.on("/feed", [this]() {
                        capture();
                    });

                    server.on("/ycbcr", [this]() {
                        this->onYCbCr();
                    });

                    server.begin(httpPort);

                    return setErrorMessage("");
                }

                /**
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    String ip = cam->getIP();

                    return
                            String(F("YCbCr Feed webpage available at http://"))
                            + ip
                            + (httpPort != 80 ? String(':') + httpPort : "")
                            + ". JPEG feed available at /feed. YCbCr available at /ycbcr";
                }

            protected:

                /**
                 *
                 * @return
                 */
                void onYCbCr() {
                    if (!cam->captured())
                        return;

                    auto client = server.client();
                    const uint16_t lenY = jpegDecoder->luma.length;
                    const uint16_t lenCb = jpegDecoder->cb.length;
                    const uint16_t lenCr = jpegDecoder->cr.length;

                    client.println(F("HTTP/1.1 200 OK"));
                    client.println(F("Content-Type: application/octet-stream"));
                    client.println(F("Connection: close"));
                    client.print(F("Content-Length: "));
                    client.println(lenY + lenCb + lenCr);
                    client.print(F("X-Width: "));
                    client.println(jpegDecoder->getWidth() / 8);
                    client.print(F("X-Height: "));
                    client.println(jpegDecoder->getHeight() / 8);
                    client.print(F("X-Luma: "));
                    client.println(lenY);
                    client.print(F("X-Cb: "));
                    client.println(lenCb);
                    client.print(F("X-Cr: "));
                    client.println(lenCr);
                    client.println();
                    client.write(jpegDecoder->luma.pixels, lenY);
                    client.write(jpegDecoder->cb.pixels, lenCb);
                    client.write(jpegDecoder->cr.pixels, lenCr);
                }

                /**
                 *
                 * @return
                 */
                bool onIndex() {
                    addCSS(F(R"===(
                        .flex > div {flex: 1;}
                    )==="));

                    addHTML(F(R"===(
                        <div id="app">
                            <div id="canvas">
                                <img id="feed" />
                                <div class="flex">
                                    <div><canvas id="y"></canvas><h4>Y</h4></div>
                                    <div><canvas id="cb"></canvas><h4>Cb</h4></div>
                                    <div><canvas id="cr"></canvas><h4>Cr</h4></div>
                                </div>
                            </div>
                        </div>
                    )==="));

                    initJpegFeedScript();
                    initYCbCrScript();
                    flush();

                    return true;
                }

                /**
                 *
                 */
                void initYCbCrScript() {
                    addJS(F(R"===(
                        function execYCbCr() {
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
                                fetch("/ycbcr")
                                    .then(res => {
                                        if (res.headers.get("X-Width")) {
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
                            }

                            drawYCbCr()
                        }
                    )==="));

                    onDOMContentLoaded("execYCbCr");
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_YCBCRHTTP_H
