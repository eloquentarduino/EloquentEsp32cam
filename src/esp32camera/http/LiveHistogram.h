//
// Created by Simone on 17/12/22.
//

#ifndef ELOQUENTESP32CAM_LIVEHISTOGRAM_H
#define ELOQUENTESP32CAM_LIVEHISTOGRAM_H

#include "../Cam.h"
#include "../JpegDecoder.h"
#include "../hist/HistogramYCbCr.h"
#include "../../traits/http/ServesJpegFeed.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            class LiveHistogram : public ServesJpegFeed {
            public:

                /**
                 *
                 * @param camera
                 * @param jpegDecoder
                 * @param histogram
                 */
                LiveHistogram(Cam& camera, JpegDecoder& decoder, Hist::HistogramYCbCr& histogram, uint16_t port = 80) :
                    ServesJpegFeed(camera, decoder, port),
                    hist(&histogram),
                    httpPort(port) {

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

                    server.on("/hist", [this]() {
                        return this->onHist();
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
                            String(F("Live Histogram webpage available at http://"))
                            + ip
                            + (httpPort != 80 ? String(':') + httpPort : "")
                            + ". JPEG feed available at /feed. JSON histogram available at /hist";
                }

            protected:

                /**
                 *
                 * @return
                 */
                bool onFeed() {
                    if (!capture())
                        return false;

                    hist->update(*jpegDecoder);

                    return true;
                }

                /**
                 *
                 * @return
                 */
                bool onHist() {
                    server.send(200, "application/json", hist->toJson());
                    return true;
                }

                /**
                 *
                 * @return
                 */
                bool onIndex() {
                    server.sendContent(F(R"===(
                        <style>
                            #app {display: flex;}
                            #histograms {display: flex; flex-direction: column;}
                            .hist {position: relative; display: flex; height: 100px; align-items: end; margin-bottom: 1rem; margin-left: 1rem; padding: 0.5rem; background: #212121; }
                            .bar { width: 20px; background: orange; border: 1px solid orange; }
                            #y:after, #cb:after, #cr:after {position: absolute; color: white; font-weight: bold; top: 0.5rem; right: 0.5rem; }
                            #y:after {content: "Y";}
                            #cb:after {content: "Cb";}
                            #cr:after {content: "Cr";}
                        </style>
                        <div id="app">
                            <div id="canvas">
                                <img id="feed" />
                            </div>
                            <div id="histograms">
                                <div class="hist" id="y"></div>
                                <div class="hist" id="cb"></div>
                                <div class="hist" id="cr"></div>
                            </div>
                        </div>
                    )==="));

                    addJpegFeedScript();
                    addHistScript();
                    server.sendContent(F(""));

                    return true;
                }

                /**
                 *
                 */
                void addHistScript() {
                    server.sendContent(F(R"===(
                        <script>
const y = document.getElementById("y")
const cb = document.getElementById("cb")
const cr = document.getElementById("cr")

function drawHist(el, values) {
    const max = Math.max(...values)
    const sum = values.reduce((sum, v) => sum + v, 0)
    const withIdx = values.map((v, i) => ({i, v: v / sum}))
    const withIdxSorted = withIdx.sort((a, b) => b.v - a.v)
    let cumsum = 0
    let greatest = []

    for (let _ of withIdxSorted) {
        cumsum += _.v

        greatest.push(_.i)

        if (cumsum >= 0.75)
            break
    }

    console.log('greatest of', el.id, greatest)

    if (el.children.length != values.length) {
        el.innerHTML = values.map(() => `<div class="bar"></div>`).join("")
    }

    values.forEach((v, i) => el.children[i].style.height = `${v * 100 / max}%`)
}

function drawHistograms() {
    try {
        fetch("/hist")
            .then(res => res.json())
            .then(histograms => {
                console.log('hist', histograms)

                if (histograms.y) {
                    drawHist(y, histograms.y)
                    drawHist(cb, histograms.cb)
                    drawHist(cr, histograms.cr)
                }

                drawHistograms()
            })
    }
    catch (e) {
        drawHistograms();
    }
}

setTimeout(drawHistograms, 2000)
</script>
                    )==="));
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_LIVEHISTOGRAM_H
