//
// Created by Simone on 17/12/22.
//

#ifndef ELOQUENTESP32CAM_LIVECOLORPICKER_H
#define ELOQUENTESP32CAM_LIVECOLORPICKER_H

#include "../../traits/http/ServesJpegFeed.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            class LiveColorPicker : public ServesJpegFeed {
            public:

                /**
                 *
                 * @param camera
                 * @param decoder
                 * @param port
                 */
                LiveColorPicker(Cam& camera, JpegDecoder& decoder, uint16_t port = 80) :
                    ServesJpegFeed(camera, decoder, port) {

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
                        return this->onFeed();
                    });

                    server.on("/pick", [this]() {
                        return this->onPick();
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
                            String(F("Live Color Picker webpage available at http://"))
                            + ip
                            + (httpPort != 80 ? String(':') + httpPort : "")
                            + ". JPEG feed available at /feed. Color pick available at /pick?x={x}&y={y}";
                }

            protected:

                /**
                 *
                 * @return
                 */
                bool onFeed() {
                    if (!capture())
                        return false;

                    return true;
                }

                /**
                 *
                 * @return
                 */
                bool onPick() {
                    if (!server.hasArg("x"))
                        return this->abort("No x provided");

                    if (!server.hasArg("y"))
                        return this->abort("No y provided");

                    const size_t x = server.arg("x").toInt() / 8;
                    const size_t y = server.arg("y").toInt() / 8;
                    const size_t i = y * (jpegDecoder->getWidth() / 8) + x;
                    const uint8_t luma = jpegDecoder->luma.pixels[i];
                    const uint8_t cb = jpegDecoder->cb.pixels[i];
                    const uint8_t cr = jpegDecoder->cr.pixels[i];
                    const uint8_t r = luma * 1.1643835616f + cr * 1.7927410714f + -248.100994f;
                    const uint8_t g = luma * 1.1643835616f + cb * -0.2132486143f + cr * -0.5329093286f + 76.878080f;
                    const uint8_t b = luma * 1.1643835616f + cb * 2.1124017857f + -289.017566f;

                    String json =
                            String("{\"luma\":")
                            + luma
                            + ",\"cb\":"
                            + cb
                            + ",\"cr\":"
                            + cr
                            + ",\"r\":"
                            + r
                            + ",\"g\":"
                            + g
                            + ",\"b\":"
                            + b
                            + ",\"x\":"
                            + x
                            + ",\"y\":"
                            + y
                            + '}';

                    server.send(200, "application/json", json);
                    return true;
                }

                /**
                 *
                 * @return
                 */
                bool onIndex() {
                    server.sendContent(F(R"===(
                        <style>
                            #feed { cursor: crosshair; }
                        </style>
                        <div id="app">
                            <div id="canvas">
                                <img id="feed" />
                                <p>Click on any point of the image to get the pixel color in YCbCr format (it is not pixel-perfect!)</p>
                            </div>
                        </div>
                    )==="));

                    addJpegFeedScript();
                    addColorPickerScript();
                    server.sendContent(F(""));

                    return true;
                }

                /**
                 *
                 */
                void addColorPickerScript() {
                    server.sendContent(F(R"===(
                        <script>
                            const rect = feed.getBoundingClientRect()

                            feed.addEventListener("click", ev => {
                                const x = parseInt(ev.clientX - rect.left)
                                const y = parseInt(ev.clientY - rect.top)

                                try {
                                    fetch(`/pick?x=${x}&y=${y}`)
                                        .then(res => res.json())
                                        .then(color => {
                                            alert(`YCbCr = ${color.luma}, ${color.cb}, ${color.cr}\nRGB = ${color.r}, ${color.g}, ${color.b}`)
                                        })
                                } catch (e) {
                                    alert("Server error")
                                }
                            })
                        </script>
                    )==="));
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_LIVECOLORPICKER_H
