//
// Created by Simone on 04/01/23.
//

#ifndef ELOQUENTESP32CAM_PERSONDETECTORHTTP_H
#define ELOQUENTESP32CAM_PERSONDETECTORHTTP_H


#include <WebServer.h>
#include "../Cam.h"
#include "../apps/PersonDetector.h"
#include "./features/DisplaysJpegFeed.h"
#include "./features/Resources.h"



namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             *
             */
            class PersonDetectorHTTP : public HasErrorMessage {
            public:
                WebServer server;
                Applications::PersonDetector *person;

                /**
                 *
                 * @param camera
                 * @param personDetector
                 * @param port
                 */
                PersonDetectorHTTP(Applications::PersonDetector& personDetector) :
                    person(&personDetector),
                    jpeg(server, *personDetector.cam) {

                }

                /**
                 *
                 * @return
                 */
                bool begin() {
                    if (!person->cam->capture())
                        return setErrorMessage("Cannot capture from camera");

                    jpeg.onRequest([]() {
                        return true;
                    });

                    if (!jpeg.isOk())
                        return setErrorMessage(jpeg.getErrorMessage());

                    server.on("/person", [this]() {
                        person->detect();

                        if (!person->isOk()) {
                            server.send(200, "text/plain", person->getErrorMessage());
                            return;
                        }

                        String status =
                                String(person->isPerson ? "Person detected" : "No person detected")
                                + "\n > Person score: "
                                + person->person.getPersonScore()
                                  + "\n > Not person score: "
                                  + person->person.getNotPersonScore();

                        server.send(200, "text/plain", status);
                    });

                    server.on("/", [this]() {
                        jpeg.css();

                        server.sendContent(R"===(
                            <div id="app">
                                <div id="frame">
                                    <img src="/jpeg" />
                                </div>
                                <pre id="status"></pre>
                            </div>
                        )===");

                        Features::Resources js(server);
                        js.onDOMContentLoaded(R"===(
                            fetch("/person")
                                .then(res => res.text())
                                .then(status => document.getElementById("status").textContent = status)
                                .catch(err => document.getElementById("status").textContent = err)
                        )===");

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
                    return String("PersonDetectorHTTP available at ")
                        + person->cam->getAddress();
                }

                /**
                 *
                 */
                void handle() {
                    server.handleClient();
                }

            protected:
                Features::DisplaysJpegFeed jpeg;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_PERSONDETECTORHTTP_H
