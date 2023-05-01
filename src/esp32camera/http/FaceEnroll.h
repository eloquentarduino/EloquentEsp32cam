//
// Created by Simone on 17/11/22.
//

#ifndef ELOQUENTESP32CAM_FACE_ENROLL_SERVER_H
#define ELOQUENTESP32CAM_FACE_ENROLL_SERVER_H

#include <WebServer.h>
#include "../apps/FaceDetector.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             * Display live camera feed with motion super-imposed
             */
            class FaceEnroll : public HasErrorMessage {
            public:
                uint16_t port;

                /**
                 *
                 * @param cam
                 * @param httpPort
                 */
                FaceEnroll(FaceDetection& faces, uint16_t httpPort = 80) :
                        _faces(&faces),
                        _cam(faces.cam),
                        port(httpPort),
                        _indexServer(httpPort) {
                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    _indexServer.on("/", [this]() {
                        _indexServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
                        _indexServer.send(200, "text/html", "");
                        _indexServer.sendContent(F(R"===(
<style>
#frame {position: relative; display: inline-block;}
.error { color: red; }
form { margin-top: 10px; }
</style>
<div id="app">
    <div id="frame">
        <img id="feed" />
    </div>
    <div>
        <form action="/enroll" method="POST">
            <input type="text" placeholder="Person name" />
            <button id="enroll" onclick="enroll()">Enroll face</button>
        </form>
    </div>
    <div>
        <pre id="info"></pre>
    </div>
</div>
<script>
const feed = document.getElementById("feed");
const info = document.getElementById("info");
</script>
<script>
function enroll() {
    let isOk

    fetch('/enroll')
        .then(res => {
            isOk = res.ok
            return res.text()
        })
        .then(res => {
            isOk ? info.classList.remove('error') : info.classList.add('error')
            info.textContent = res
        })
}
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
setTimeout(() => drawFeed(), 1000)
</script>
)==="));
                        _indexServer.sendContent(F(""));
                        return true;
                    });

                    _indexServer.on("/feed", [this]() {
                        if (!_cam->capture()) {
                            _indexServer.send(500, "text/plain", "No frame");
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

                    _indexServer.on("/enroll", [this]() {
                        if (!_cam->captured()) {
                            _indexServer.send(500, "text/plain", "No frame");
                            return false;
                        }

                        if (_faces->enroll()) {
                            _indexServer.send(200, "text/plain", "Enrolled");
                            return true;
                        }

                        _indexServer.send(500, "text/plain", _faces->getErrorMessage());
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
                            String(F("Enroll Face webpage available at http://"))
                            + ip
                            + (port != 80 ? String(':') + port : "");
                }

            protected:
                Cam* _cam;
                FaceDetection* _faces;
                WebServer _indexServer;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_FACE_ENROLL_SERVER_H
