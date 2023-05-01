//
// Created by Simone on 31/12/22.
//

#ifndef ELOQUENTESP32CAM_DRAWSBBOXESONIMAGE_H
#define ELOQUENTESP32CAM_DRAWSBBOXESONIMAGE_H

#include <WebServer.h>
#include "./Resources.h"
#include "../../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Features {

                /**
                 * Draw bboxes on live feed from API
                 */
                class DrawsBBoxesOnImage : public HasErrorMessage {
                public:
                    WebServer *server;
                    Resources resources;

                    /**
                     *
                     * @param webServer
                     */
                    DrawsBBoxesOnImage(WebServer& webServer) :
                        server(&webServer),
                        resources(webServer) {

                    }

                    /**
                     *
                     * @tparam Callback
                     * @param callback
                     */
                    template<typename Callback>
                    void onRequest(Callback callback) {
                        server->on("/bboxes", [callback]() {
                            callback();
                        });
                    }

                    /**
                     * Add CSS to webpage
                     */
                    void css() {
                        resources.css(".bbox {position: absolute; border: 2px solid #000; }");
                    }

                    /**
                     * Add JS to webpage
                     */
                    void js() {
                        resources.onDOMContentLoaded(R"===(
                            const frame = document.getElementById("frame")
                            let container = document.getElementById("bboxes-container")
                            const palette = ["#f1c40f", "#16a085", "#e74c3c", "#2c3e50", "#2980b9"]

                            function initBBoxes() {
                                if (!container) {
                                    container = document.createElement("div")
                                    container.id = "bboxes-container"
                                    frame.appendChild(container)
                                }

                                drawBBoxes()
                            }

                            function drawBBoxes() {
                                try {
                                    fetch("/bboxes")
                                        .then(res => res.json())
                                        .then(bboxes => bboxes.concat([...new Array(10 - bboxes.length)]))
                                        .then(bboxes => {
                                            bboxes.forEach((bbox, i) => {
                                                let e = container.children[i]

                                                if (!e) {
                                                    e = document.createElement("div")
                                                    e.classList.add("bbox")
                                                    container.appendChild(e)
                                                }

                                                if (bbox) {
                                                    e.style.left = `${bbox.left}px`
                                                    e.style.top = `${bbox.top}px`
                                                    e.style.width = `${bbox.right - bbox.left}px`
                                                    e.style.height = `${bbox.bottom - bbox.top}px`
                                                    e.style.borderWidth = '2px'
                                                    e.style.borderColor = palette[i % palette.length]
                                                }
                                                else {
                                                    e.style.left = '0px'
                                                    e.style.top = '0px'
                                                    e.style.width = '0px'
                                                    e.style.height = '0px'
                                                    e.style.borderWidth = '0px'
                                                }
                                            })

                                            drawBBoxes()
                                        })
                                } catch (e) { drawBBoxes() }
                            }

                            initBBoxes()
                        )===");
                    }
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_DRAWSBBOXESONIMAGE_H
