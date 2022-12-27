//
// Created by Simone on 20/12/22.
//

#ifndef ELOQUENTESP32CAM_DRAWSMASKONJPEGFEED_H
#define ELOQUENTESP32CAM_DRAWSMASKONJPEGFEED_H

#include "./ServesJpegFeed.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            class DrawsMaskOnJpegFeed : public ServesJpegFeed {
            protected:

                /**
                 *
                 * @param cam
                 * @param decoder
                 * @param port
                 */
                DrawsMaskOnJpegFeed(
                        Cam &cam,
                        JpegDecoder &decoder,
                        uint16_t port = 80) : ServesJpegFeed(cam, decoder, port) {

                }

            protected:

                /**
                 * Setup mask scripts (css + js)
                 */
                void initMaskScripts() {
                    initWebpage();

                    addCSS(R"===(
                        .grid {position: absolute; top: 0; left: 0; right: 0; bottom: 0; display:grid; width: 100%; height: 100%; }
                        .cell.foreground { background: rgba(255, 255, 0, 0.5); }
                    )===");

                    addJS(R"===(
                        function initMasks() {
                            const numMasks = (window.APP.masks || [0]).length

                            // set custom colors, if available
                            ;(window.APP.masks || []).forEach((mask, i) => {
                                const defaults = ["#f1c40f", "#16a085", "#e74c3c", "#2c3e50", "#2980b9"]
                                const hex = mask.hex || defaults[i % defaults.length]
                                const style = document.createElement("style")

                                style.innerText = `.grid:nth-child(${i+1}) .cell.foreground { background: ${mask.hex}F0; }`
                                document.body.appendChild(style)
                            })

                            // create grids
                            ;[...new Array(numMasks)].forEach(() => {
                                const grid = document.createElement("div")

                                grid.classList.add("grid")
                                grid.style["grid-template-columns"] = `repeat(${window.APP.constants.maskWidth}, 1fr)`
                                grid.style["grid-template-rows"] = `repeat(${window.APP.constants.maskHeight}, 1fr)`

                                // create cells of grid
                                ;[...new Array(window.APP.constants.maskWidth * window.APP.constants.maskHeight)].forEach(() => {
                                    const cell = document.createElement("span")
                                    cell.className = "cell"
                                    grid.appendChild(cell)
                                })

                                document.getElementById("grid-container").appendChild(grid)
                            })

                            drawMaskOverlay();
                        }

                        function drawMaskOverlay() {
                            try {
                                fetch("/masks")
                                    .then(res => window.APP.listeners && window.APP.listeners.onMask ? window.APP.listeners.onMask(res) : res.text())
                                    .then(masks => {
                                        // one mask per line
                                        masks.split("\n").map((mask, i) => {
                                            if (!mask.trim())
                                                return;

                                            try {
                                                mask.split("").forEach((isForeground, j) => {
                                                    const classList = document.getElementById("grid-container").children[i].children[j].classList
                                                    isForeground == "1" ? classList.add("foreground") : classList.remove("foreground")
                                                })
                                            } catch (e) {
                                                console.error(e)
                                            }
                                        })

                                        drawMaskOverlay()
                                })
                            } catch (e) {
                                drawMaskOverlay()
                            }
                        }
                     )===");

                    onDOMContentLoaded("initMasks");
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_DRAWSMASKONJPEGFEED_H
