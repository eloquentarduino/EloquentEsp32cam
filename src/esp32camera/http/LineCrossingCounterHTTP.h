//
// Created by Simone on 07/12/22.
//

#ifndef ELOQUENTESP32CAM_LINECROSSINGCOUNTERCOUNTER_H
#define ELOQUENTESP32CAM_LINECROSSINGCOUNTERCOUNTER_H


#include "../Cam.h"
#include "../motion/Detector.h"
#include "../apps/LineCrossingCounter.h"
#include "../../traits/http/ServesJpegFeed.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {

            /**
             * Debug line crossing counter
             */
            class LineCrossingCounterHTTP : public ServesJpegFeed {
            public:
                int httpPort;
                Motion::Detector *detector;
                Applications::LineCrossingCounter *counter;

                /**
                 *
                 * @param camera
                 * @param decoder
                 * @param motionDetector
                 * @param crossingCounter
                 * @param port
                 */
                LineCrossingCounterHTTP(
                        Cam& camera,
                        JpegDecoder& decoder,
                        Motion::Detector& motionDetector,
                        Applications::LineCrossingCounter& crossingCounter,
                        uint16_t port = 80) :
                    ServesJpegFeed(camera, decoder, port),
                    detector(&motionDetector),
                    counter(&crossingCounter) {

                }

                /**
                 * Run HTTP server
                 * @return
                 */
                bool begin() {
                    server.on("/", [this]() {
                        return this->onIndex();
                    });

                    server.on("/feed", [this]() {
                        return this->onFeed();
                    });

                    server.on("/status", [this]() {
                        return this->onStatus();
                    });

                    server.begin();

                    return setErrorMessage("");
                }

                /**
                 * Handle client connection
                 */
                void handle() {
                    server.handleClient();
                }

                /**
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    String ip = cam->getIP();

                    return
                        String(F("Line Crossing Counter Feed webpage available at http://"))
                        + ip
                        + (httpPort != 80 ? String(':') + httpPort : "")
                        + ". JPEG feed available at /feed. JSON status available at /status";
                }

            protected:
                /**
                 *
                 * @return
                 */
                bool onIndex() {
                    // reset motion detector and counter on new connection
                    detector->forget();
                    counter->forget();

                    // update counter parameters from query string
                    for (int i = 0; i < server.args(); i++) {
                        counter->set(server.argName(i), server.arg(i).toFloat());
                    }

                    refresh();
                    initWebpage();

                    addHTML(F(R"===(
                        <div id="app">
                            <div id="canvas">
                                <img id="feed" />
                                <div id="line-canvas">
                                    <div id="tol"></div>
                                    <div id="line"></div>
                                </div>
                            </div>
                            <pre id="configText"></pre>
                            <pre id="statusText"></pre>
                        </div>
                    )==="));

                    initJpegFeedScript();
                    registerConfigScript();
                    addLineScript();
                    registerStatusScript();

                    flush();

                    return true;
                }

                /**
                 *
                 * @return
                 */
                bool onFeed() {
                    if (!capture())
                        return false;

                    if (detector->update(jpegDecoder->luma)) {
                        counter->update();
                        counter->crossedFromLeftToRight();
                        counter->crossedFromRightToLeft();
                    }

                    return true;
                }

                /**
                 *
                 * @return
                 */
                bool onStatus() {
                    server.send(200, "application/json", counter->toJson());
                    return true;
                }

                /**
                 *
                 */
                void registerConfigScript() {
                    js.onDOMContentLoaded(
                            String(F("document.getElementById('configText').textContent = '"))
                            + counter->getCurrentConfig()
                            + "'");
                }

                /**
                 *
                 */
                void addLineScript() {
                    server.sendContent(F("<script>var lineX = "));
                    server.sendContent(String(counter->getX(cam->getWidth())));
                    server.sendContent(F(";var bandWidth = "));
                    server.sendContent(String(counter->getBandWidth()));
                    server.sendContent(F("</script>"));
                    server.sendContent(F(R"===(
                        <style>
                        #line {position: absolute; top: 0; bottom: 0; width: 3px; background: red;}
                        .band {position: absolute; top: 0; bottom: 0; background: rgba(255, 255, 255, 0.25); }
                        </style>
                        <script>
                        const line = document.getElementById("line")
                        const tol = document.getElementById("tol")
                        ;(function drawLine() {
                            line.style.left = `${lineX - 1}px`

                            ;[-3, -2, -1, 0, 1, 2].forEach(n => {
                                const l = document.createElement('div')
                                const offset = n < 0 ? -1 : 1
                                l.classList.add('band')
                                l.style.left = `${lineX + n * bandWidth + offset}px`
                                l.style.width = `${bandWidth}px`
                                tol.appendChild(l)
                            })
                        })()
                        var bands = [].slice.call(document.querySelectorAll('.band'))
                        </script>
                    )==="));
                }

                /**
                 *
                 */
                void registerStatusScript() {
                    js.onDOMContentLoaded(F(R"===(
                        const statusText = document.getElementById("statusText")

                        function updateStatus() {
                            try {
                                fetch("/status")
                                    .then(res => res.json())
                                    .then(status => {
                                        const maxAge = 20

                                        // draw bands
                                        if (status && status.ages && status.ages.forEach) {
                                            status.ages.forEach((age, i) => {
                                                const opacity = 1 - Math.min(age, maxAge) / maxAge
                                                const hue = Math.round(60 * Math.min(age, maxAge) / maxAge)
                                                //if (bands[i]) bands[i].style.backgroundColor = `rgba(255, 255, 255, ${opacity / 2})`;
                                                if (bands[i]) bands[i].style.backgroundColor = `hsla(${hue}, 100%, 50%, ${opacity / 2})`;
                                            })
                                        }

                                        // print status text
                                        statusText.textContent = `${status.ltr} crosses from left to right.\n${status.rtl} crosses from right to left.`

                                        updateStatus()
                                    })
                            } catch (e) {
                                updateStatus()
                            }
                        }

                        updateStatus()
                    )==="));
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_LINECROSSINGCOUNTERCOUNTER_H
