//
// Created by Simone on 24/12/22.
//

#ifndef ELOQUENTESP32CAM_PICKSCOLORFROMIMAGE_H
#define ELOQUENTESP32CAM_PICKSCOLORFROMIMAGE_H

#include <WebServer.h>
#include "../../JpegDecoder.h"
#include "./Script.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Features {

                /**
                 * Pick color from image by clicking on the image feed
                 */
                class PicksColorFromImage {
                public:

                    /**
                     *
                     * @param webServer
                     * @param jpegDecoder
                     */
                    PicksColorFromImage(WebServer& webServer, JpegDecoder* jpegDecoder) :
                        _server(&webServer),
                        _jpeg(jpegDecoder) {

                    }

                    /**
                     * Add color picker route
                     */
                    void registerRoute() {
                        _server->on("/colorpicker", [this]() {
                            uint16_t x = _server->arg("x").toInt() / 8;
                            uint16_t y = _server->arg("y").toInt() / 8;
                            uint16_t i = y * _jpeg->luma.width + x;

                            uint8_t l = _jpeg->luma.pixels[i];
                            uint8_t cb = _jpeg->cb.pixels[i];
                            uint8_t cr = _jpeg->cr.pixels[i];

                            _server->sendContent(F("{\"l\":"));
                            _server->sendContent(String(l));
                            _server->sendContent(F(", \"cb\":"));
                            _server->sendContent(String(cb));
                            _server->sendContent(F(", \"cr\":"));
                            _server->sendContent(String(cr));
                            _server->sendContent(F("}"));
                            _server->sendContent("");
                        });
                    }

                    /**
                     * Add JS code for color picker
                     */
                    void registerJS()
                    {
                        Script js(_server);

                        js.onDOMContentLoaded(R"===(
                            const frame = document.getElementById("frame")

                            frame.addEventListener("click", ev => {
                                const rect = frame.getBoundingClientRect()
                                const x = parseInt(ev.clientX - rect.left)
                                const y = parseInt(ev.clientY - rect.top)

                                try {
                                    fetch(`/colorpicker?x=${x}&y=${y}`)
                                        .then(res => res.json())
                                        .then(color => alert(`YCbCr at (x=${x}, y=${y}) is (${color.l}, ${color.cb}, ${color.cr})`))
                                } catch (e) {}
                            })
                        )===");
                    }

                protected:
                    WebServer *_server;
                    JpegDecoder *_jpeg;
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_PICKSCOLORFROMIMAGE_H
