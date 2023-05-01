//
// Created by Simone on 20/12/22.
//

#ifndef ELOQUENTESP32CAM_COLORBLOBDETECTORHTTP_H
#define ELOQUENTESP32CAM_COLORBLOBDETECTORHTTP_H

#include "../../traits/http/DrawsMaskOnJpegFeed.h"
#include "./features/PicksColorFromImage.h"
#include "../apps/ColorBlobDetector.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             *
             */
            class ColorBlobDetectorHTTP : public DrawsMaskOnJpegFeed {
            public:

                /**
                 *
                 * @param cam
                 * @param decoder
                 * @param detector
                 * @param port
                 */
                ColorBlobDetectorHTTP(Cam& cam, JpegDecoder& decoder, Applications::ColorBlobDetector& detector, uint16_t port = 80) :
                        DrawsMaskOnJpegFeed(cam, decoder, port),
                        _detector(&detector),
                        _colorPicker(server, jpegDecoder) {
                }

                /**
                 * Stack multiple detectors
                 *
                 * @param detector
                 */
                void addDetector(Applications::ColorBlobDetector& detector) {
                    _detector->chain(detector);
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    if (!probeCamera())
                        return false;

                    server.on("/", [this]() {
                        // update detector parameters from query string
                        for (int i = 0; i < server.args(); i++) {
                            _detector->set(server.argName(i), server.arg(i).toFloat());
                        }

                        refresh();
                        initWebpage();
                        addJsRawVariable("maskWidth", jpegDecoder->cb.width);
                        addJsRawVariable("maskHeight", jpegDecoder->cb.height);

                        declareMasks();
                        initMaskScripts();
                        initJpegFeedScript();
                        initBboxScript();

                        //_colorPicker.registerCSS();
                        _colorPicker.registerJS();

                        addCSS(R"===(
                            .bbox {position: absolute; border: 2px solid #000; }
                        )===");

                         addHTML(R"===(
                            <div id="app">
                                <div id="frame">
                                    <img id="feed" />
                                    <div id="grid-container"></div>
                                </div>
                            </div>
                         )===");

                         addHTML(F("<pre>Target YCbCr: ("));
                         addHTML(String(_detector->getLuma()));
                         addHTML(F(", "));
                         addHTML(String(_detector->getCb()));
                         addHTML(F(", "));
                         addHTML(String(_detector->getCr()));
                         addHTML(F(")\nTollerance: "));
                         addHTML(String(_detector->getTol()));
                         addHTML(F("\nMin. area: "));
                         addHTML(String(_detector->getMinArea()));
                         addHTML(F("</pre>"));

                         flush();
                    });

                    server.on("/feed", [this]() {
                        this->capture();
                    });

                    server.on("/masks", [this]() {
                        this->onMasks();
                    });

                    server.on("/bboxes", [this]() {
                        this->onBboxes();
                    });

                    _colorPicker.registerRoute();

                    server.begin(httpPort);

                    return true;
                }

                /**
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    return
                            String(F("Color Blob Detector webpage available at "))
                            + cam->getAddress(httpPort)
                            + ". JPEG feed available at /feed. Blob mask available at /mask";
                }

            protected:
                Applications::ColorBlobDetector *_detector;
                Features::PicksColorFromImage _colorPicker;

                /**
                 *
                 */
                void onMasks() {
                    Applications::ColorBlobDetector *detector = _detector;

                    while (detector != NULL) {
                        detector->detect(*jpegDecoder);
                        server.sendContent(detector->toString());
                        server.sendContent("\n");
                        detector = detector->next;
                    }

                    flush();
                }

                /**
                 *
                 */
                void onBboxes() {
                    Applications::ColorBlobDetector *detector = _detector;

                    while (detector != NULL) {
                        server.sendContent(detector->isOk() ? detector->bboxAsJson() : String(""));
                        server.sendContent("\n");
                        detector = detector->next;
                    }

                    flush();
                }

                /**
                 * Instantiate masks in JS
                 */
                void declareMasks() {
                    uint8_t i = 0;
                    Applications::ColorBlobDetector *detector = _detector;

                    server.sendContent(F("<script>window.APP.masks = ["));

                    while (detector != NULL) {
                        if (i > 0)
                            server.sendContent(",");

                        server.sendContent(F("{i: "));
                        server.sendContent(String(i));
                        server.sendContent(F(", hex: \"#"));
                        server.sendContent(detector->toRGB());
                        server.sendContent(F("\"}"));

                        detector = detector->next;
                        i += 1;
                    }

                    server.sendContent(F("]</script>\n"));
                }

                void initBboxScript() {
                    addJS(R"===(
                        function execBboxes() {
                            const frame = document.getElementById("frame")

                            ;(window.APP.masks || [0]).forEach(mask => {
                                const bbox = document.createElement("div")

                                bbox.classList.add("bbox")
                                frame.appendChild(bbox)
                            })

                            const bboxElements = [].slice.call(document.querySelectorAll(".bbox"))
                            console.log('bbox elements', bboxElements)

                            function drawBboxes() {
                                try {
                                    fetch("/bboxes")
                                        .then(res => res.text())
                                        .then(bboxes => {
                                            // one bbox per line
                                            bboxes.split("\n").forEach((bbox, i) => {
                                                if (!bboxElements[i])
                                                    return;

                                                if (!bbox) {
                                                    bboxElements[i].style.width = "0"
                                                    bboxElements[i].style.height = "0"
                                                    bboxElements[i].style.top = "0"
                                                    bboxElements[i].style.left = "0"
                                                    return
                                                }

                                                bbox = JSON.parse(bbox)

                                                if (bbox.left || bbox.right) {
                                                    const e = bboxElements[i]

                                                    e.style.top = `${bbox.top}px`
                                                    e.style.left = `${bbox.left}px`
                                                    e.style.width = `${bbox.right - bbox.left}px`
                                                    e.style.height = `${bbox.bottom - bbox.top}px`
                                                }
                                            })

                                            drawBboxes()
                                        })
                                } catch (e) { drawBboxes() }
                            }

                            drawBboxes()
                        }
                    )===");

                    onDOMContentLoaded("execBboxes");
                }

            };
        }
    }
}

#endif //ELOQUENTESP32CAM_COLORBLOBDETECTORHTTP_H
