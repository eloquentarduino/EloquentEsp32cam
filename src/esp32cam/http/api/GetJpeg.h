//
// Created by Simone on 17/02/23.
//

#ifndef ELOQUENTESP32CAM_GETJPEG_H
#define ELOQUENTESP32CAM_GETJPEG_H

#include "../../../esp32cam/Cam.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Api {
                class GetJpeg {
                public:
                    Cam *cam;

                    /**
                     *
                     * @param camera
                     */
                    GetJpeg(Cam& camera) :
                        cam(&camera) {

                    }

                    /**
                     *
                     * @param webServer
                     */
                     String registerTo(WebServer& webServer) const {
                        webServer.on("/jpeg", [this, &webServer]() {
                            if (!this->cam->capture()) {
                                webServer.send(500, "text/plain", this->cam->getErrorMessage());
                                return;
                            }

                            WiFiClient client = webServer.client();

                            client.println(F("HTTP/1.1 200 OK"));
                            client.println(F("Content-Type: image/jpeg"));
                            client.println(F("Connection: close"));
                            client.println(F("Access-Control-Allow-Origin: *"));
                            client.print(F("Content-Length: "));
                            client.println(this->cam->frame->len);
                            client.println();
                            client.write(this->cam->frame->buf, this->cam->frame->len);
                        });

                        return "/jpeg -> Get JPEG frame";
                    }
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_GETJPEG_H
