//
// Created by Simone on 17/02/23.
//

#ifndef ELOQUENTESP32CAM_GETGRAYSCALE_H
#define ELOQUENTESP32CAM_GETGRAYSCALE_H

#ifndef COLLECT_IMAGE_WIDTH
#define COLLECT_IMAGE_WIDTH 96
#endif

#ifndef COLLECT_IMAGE_HEIGHT
#define COLLECT_IMAGE_HEIGHT COLLECT_IMAGE_WIDTH
#endif

#include "../../../esp32cam/JpegDecoderGray.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            namespace Api {
                class GetGrayscale {
                public:
                    Cam *cam;
                    JpegDecoderGray decoder;

                    /**
                     *
                     * @param camera
                     */
                    GetGrayscale(Cam& camera) :
                    cam(&camera) {

                    }

                    /**
                     *
                     * @param webServer
                     */
                    String registerTo(WebServer& webServer) const {
                        JpegDecoderGray *dec = (JpegDecoderGray*) &decoder;

                        webServer.on("/gray", [this, dec, &webServer]() {
                            if (!this->cam->capture()) {
                                webServer.send(500, "text/plain", this->cam->getErrorMessage());
                                return;
                            }

                            if (!dec->decode(*cam)) {
                                webServer.send(500, "text/plain", "JPEG decode error");
                                return;
                            }

                            const uint16_t width = dec->gray.width;
                            const uint16_t height = dec->gray.height;
                            const uint16_t y0 = (height - COLLECT_IMAGE_HEIGHT) / 2;
                            const uint16_t x0 = (width - COLLECT_IMAGE_WIDTH) / 2;
                            const char *alphabet = "0123456789ABCDEF";

                            WiFiClient client = webServer.client();

                            client.println(F("HTTP/1.1 200 OK"));
                            client.println(F("Content-Type: text/plain"));
                            client.println(F("Connection: close"));
                            client.println(F("Access-Control-Allow-Origin: *"));
                            client.print(F("Content-Length: "));
                            client.println(COLLECT_IMAGE_WIDTH * COLLECT_IMAGE_HEIGHT * 2);
                            client.println();

                            for (size_t y = y0; y < y0 + COLLECT_IMAGE_HEIGHT; y++) {
                                const size_t offset = y * width;
                                char line[COLLECT_IMAGE_WIDTH * 2 + 1];
                                uint8_t j = 0;

                                for (size_t x = x0; x < x0 + COLLECT_IMAGE_WIDTH; x++) {
                                    const uint8_t pixel = dec->gray.pixels[offset + x];

                                    line[j++] = alphabet[pixel >> 4];
                                    line[j++] = alphabet[pixel & 0xF];
                                }

                                line[j] = '\0';
                                client.print(line);
                            }

                            client.println();
                            //webServer.sendContent("");
                        });

                        return "/gray -> Get grayscale frame, 1/8th of original size";
                    }
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_GETGRAYSCALE_H
