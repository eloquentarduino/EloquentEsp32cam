//
// Created by Simone on 31/12/22.
//

#ifndef ELOQUENTESP32CAM_FACEDETECTORHTTP_H
#define ELOQUENTESP32CAM_FACEDETECTORHTTP_H


#include <WebServer.h>
#include "../Cam.h"
#include "../apps/FaceDetector.h"
#include "./features/DisplaysJpegFeed.h"
#include "./features/DrawsBBoxesOnImage.h"



namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             *
             */
            class FaceDetectorHTTP : public HasErrorMessage {
            public:
                WebServer server;
                Applications::FaceDetector *faces;

                /**
                 *
                 * @param camera
                 * @param faces
                 * @param port
                 */
                FaceDetectorHTTP(Applications::FaceDetector& faceDetector) :
                    faces(&faceDetector),
                    jpeg(server, *faceDetector.cam),
                    drawsBBoxes(server) {

                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    jpeg.onRequest([this]() {
                        faces->detect();
                    });

                    drawsBBoxes.onRequest([this]() {
                        server.send(200, "application/json", faces->toJson());
                    });

                    if (!jpeg.isOk())
                        return setErrorMessage(jpeg.getErrorMessage());

                    if (!drawsBBoxes.isOk())
                        return setErrorMessage(drawsBBoxes.getErrorMessage());

                    server.on("/", [this]() {
                        jpeg.css();
                        drawsBBoxes.css();

                        server.sendContent(R"===(
                            <div id="app">
                                <div id="frame">
                                    <img id="jpeg" />
                                </div>
                                <pre id="status"></pre>
                            </div>
                        )===");

                        jpeg.js();
                        drawsBBoxes.js();
                        server.sendContent("");
                    });

                    server.begin();
                    return setErrorMessage("");
                }

                /**
                 * Get endpoints of HTTP server
                 *
                 * @return
                 */
                String getWelcomeMessage() {
                    return String("FaceDetectorHTTP available at ")
                        + faces->cam->getAddress()
                        + ". JPEG feed available at /jpeg. Faces' bboxes available at /bboxes";
                }

                /**
                 *
                 */
                void handle() {
                    server.handleClient();
                }

            protected:
                Features::DisplaysJpegFeed jpeg;
                Features::DrawsBBoxesOnImage drawsBBoxes;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_FACEDETECTORHTTP_H
