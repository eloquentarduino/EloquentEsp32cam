//
// Created by Simone on 17/11/22.
//

#ifndef ELOQUENTESP32CAM_MOTIONLIVEFEED_H
#define ELOQUENTESP32CAM_MOTIONLIVEFEED_H

#include <WebServer.h>

#include "../motion/Detector.h"
#include "../motion/roi/RoI.h"
#include "../motion/localization/Localizer.h"
#include "../JpegDecoder.h"
#include "../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             * Display live camera feed with motion super-imposed
             */
            class MotionLiveFeed : public HasErrorMessage {
            public:
                uint16_t port;

                /**
                 *
                 * @param cam
                 * @param httpPort
                 */
                MotionLiveFeed(Cam& cam, Motion::Detector& detector, uint16_t httpPort = 80) :
                    _cam(&cam),
                    _detector(&detector),
                    _roi(NULL),
                    _localizer(NULL),
                    port(httpPort),
                    _indexServer(httpPort) {
                }

                /**
                 * Display RoI on top of image
                 * @param roi
                 */
                void addRoI(Motion::RoI::RegionOfInterest& roi) {
                    if (_roi == NULL)
                        _roi = &roi;
                    else
                        _roi->chain(roi);
                }

                /**
                 * Display localization
                 * @param localizer
                 */
                void localizeWith(Motion::Localization::Localizer& localizer) {
                    _localizer = &localizer;
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    _indexServer.on("/", [this]() {
                        _cam->capture();
                        _detector->forget();

                        // update detector parameters from query string
                        for (int i = 0; i < _indexServer.args(); i++) {
                            _detector->set(_indexServer.argName(i), _indexServer.arg(i).toFloat());
                        }

                        _indexServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
                        _indexServer.send(200, "text/html", "");
                        _indexServer.sendContent(F("<script>var width = "));
                        _indexServer.sendContent(String(_cam->getWidth()));
                        _indexServer.sendContent(F("; var height = "));
                        _indexServer.sendContent(String(_cam->getHeight()));
                        _indexServer.sendContent(F("; var detectorConfig = `"));
                        _indexServer.sendContent(_detector->getCurrentConfig());
                        _indexServer.sendContent(F("`; var algorithmConfig = `"));
                        _indexServer.sendContent(_detector->algorithm->getCurrentConfig());
                        _indexServer.sendContent(F("`</script>"));
                        _indexServer.sendContent(F("<script>var rois = []</script>"));

                        Motion::RoI::RegionOfInterest *roi = _roi;

                        while (roi != NULL) {
                            roi->setBoundaries(_cam->getWidth() / 8, _cam->getHeight() / 8);
                            _indexServer.sendContent(F("<script>rois.push((ctx) => {"));
                            _indexServer.sendContent(roi->drawToCanvas());
                            _indexServer.sendContent(F("})</script>"));

                            roi = roi->next();
                        }

                        _indexServer.sendContent(F(R"===(
<style>
#frame {position: relative; display: inline-block;}
#grid-container {position: absolute; top: 0; left: 0; right: 0; bottom: 0;}
#grid {display:grid; width: 100%; height: 100%; }
#canvas {position: absolute; top: 0; left: 0; width: 100%; height: 100%;}
#localization {position: absolute; background: rgba(255, 0, 0, 0.2); width: 0; height: 0;}
.cell.foreground { background: rgba(255, 255, 0, 0.5); }
</style>
<div id="app">
    <div id="frame">
        <img id="feed" />
        <div id="grid-container"><div id="grid"></div></div>
        <canvas id="canvas"></canvas>
        <div id="localization"></div>
    </div>
    <div>
        <pre id="detector-config"></pre>
        <pre id="algorithm-config"></pre>
        <pre id="localization-text"></pre>
        <pre id="motion-response"></pre>
    </div>
</div>
<script>
document.getElementById('detector-config').textContent = 'Detector config: ' + detectorConfig;
document.getElementById('algorithm-config').textContent = 'Algorithm config: ' + algorithmConfig;
</script>
<script>
var colors = ['#3498db', '#c0392b', '#16a085', '#2c3e50', '#e67e22', '#8e44ad']

const urlParams = new URLSearchParams(window.location.search);
const grid = document.getElementById("grid");
const feed = document.getElementById("feed");
const localization = document.getElementById("localization");
const localizationText = document.getElementById("localization-text");
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext('2d')
const motionResponse = document.getElementById("motion-response");

// draw grid
grid.style['grid-template-columns'] = `repeat(${width / 8}, 1fr)`
grid.style['grid-template-rows'] = `repeat(${height / 8}, 1fr)`
;[...new Array(width * height / 64)].forEach(() => {
    const cell = document.createElement('span')
    cell.className = 'cell'
    grid.appendChild(cell)
})

// draw RoIs
canvas.setAttribute("width", width);
canvas.setAttribute("height", height);
rois.forEach((draw, i) => {
    ctx.strokeStyle = colors[i % colors.length]
    ctx.lineWidth = parseInt(urlParams.get('line-width') || '4')
    draw(ctx)
})
</script>
<script>
function drawFeed() {
    fetch("/feed")
        .then(res => res.arrayBuffer())
        .then(jpeg => {
            const blob = new Blob([jpeg], { type: "image/jpeg" });
            feed.src = (window.URL || window.webkitURL).createObjectURL(blob)
            drawFeed()
        })
}

function drawMotion() {
    let statusMessages = []
    let isOk

    fetch("/motion")
        .then(res => {
            // RoIs
            statusMessages.push(res.headers.get('X-Motion-Status'))
            rois.forEach((_, i) => statusMessages.push(res.headers.get(`X-Motion-Status-Roi-${i}`)))

            // localization
            try {
                const locHeader = res.headers.get('X-Motion-Localization')
                const loc = locHeader ? JSON.parse(locHeader) : null

                localizationText.innerText = `Localized motion at: ${locHeader}`

                if (loc) {
                    localization.style.left = `${loc.x}px`
                    localization.style.top = `${loc.y}px`
                    localization.style.width = `${loc.w}px`
                    localization.style.height = `${loc.h}px`
                }
                else {
                    localization.style.left = '0px'
                    localization.style.top = '0px'
                    localization.style.width = '0px'
                    localization.style.height = '0px'
                }
            } catch (e) {}

            return res
        })
        .then(res => {
            isOk = res.ok
            return res.text()
        })
        .then(body => {
            if (isOk) {
                motionResponse.textContent = statusMessages.join("\n")
                body.split("").forEach((isForeground, i) => {
                    const classList = grid.children[i].classList
                    isForeground == '1' ? classList.add("foreground") : classList.remove("foreground")
                })
            }
            else {
                motionResponse.textContent = body
            }

            drawMotion()
        })
}

setTimeout(() => drawFeed(), 1000)
setTimeout(() => drawMotion(), 1500)
</script>
)==="));
                        _indexServer.sendContent(F(""));
                        return true;
                    });

                    _indexServer.on("/motion", [this]() {
                        if (!_detector->isTraining()) {
                            _indexServer.sendHeader(
                                    "X-Motion-Status",
                                    String(_detector->triggered() ? "Motion detected" : "Motion not detected")
                                    + " | "
                                    + _detector->getTriggerStatus());

                            // RoIs
                            uint16_t i = 0;
                            Motion::RoI::RegionOfInterest *roi = _roi;

                            while (roi != NULL) {
                                _detector->triggeredIn(*roi);

                                _indexServer.sendHeader(
                                        String("X-Motion-Status-Roi-") + i,
                                        String(roi->triggered() ? "Motion detected" : "Motion not detected")
                                        + " | "
                                        + roi->getTriggerStatus());

                                i += 1;
                                roi = roi->next();
                            }

                            // localizer
                            if (_localizer != NULL && _localizer->localize()) {
                                _indexServer.sendHeader("X-Motion-Localization", _localizer->toJson());
                            }

                            _indexServer.send(200, "text/plain", _detector->toString());
                        }
                        else
                            _indexServer.send(500, "text/plain", "Training");
                    });

                    _indexServer.on("/feed", [this]() {
                        if (!_cam->capture()) {
                            _indexServer.send(500, "text/plain", "No frame");
                            return false;
                        }

                        if (!_decoder.decode(*_cam)) {
                            _indexServer.send(500, "text/plain", _decoder.getErrorMessage());
                            return false;
                        }

                        if (!_detector->update(_decoder.luma) && !_detector->isTraining()) {
                            _indexServer.send(500, "text/plain", _detector->getErrorMessage());
                            return false;
                        }

                        WiFiClient client = _indexServer.client();
                        client.println(F("HTTP/1.1 200 OK"));
                        client.println(F("Content-Type: image/jpeg"));
                        client.println(F("Connection: close"));
                        client.print(F("Content-Length: "));
                        client.println(_cam->frame->len);

                        client.println();
                        client.write(_cam->frame->buf, _cam->frame->len);
                        return true;
                    });

                    _indexServer.begin();

                    return setErrorMessage("");
                }

                /**
                 *
                 */
                void handle() {
                    _indexServer.handleClient();
                }

                /**
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    String ip = this->_cam->getIP();

                    return
                            String(F("Motion Live Feed webpage available at http://"))
                            + ip
                            + (port != 80 ? String(':') + port : "");
                }

            protected:
                Cam* _cam;
                JpegDecoder _decoder;
                Motion::Detector* _detector;
                Motion::RoI::RegionOfInterest* _roi;
                Motion::Localization::Localizer* _localizer;
                WebServer _indexServer;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_MOTIONLIVEFEED_H
