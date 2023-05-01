//
// Created by Simone on 16/12/22.
//

#ifndef ELOQUENTESP32CAM_HISTOGRAMYCBCR_H
#define ELOQUENTESP32CAM_HISTOGRAMYCBCR_H

#include "../../traits/HasErrorMessage.h"
#include "../JpegDecoder.h"
#include "../http/HttpServer.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Hist {
            /**
             * Compute Y, Cb, Cr histograms
             */
            class HistogramYCbCr : public HasErrorMessage {
            public:
                uint16_t y[16];
                uint16_t cb[16];
                uint16_t cr[16];
                Http::HttpServer<1> http;

                /**
                 *
                 * @param resolution
                 */
                HistogramYCbCr(uint8_t resolution = 4) :
                    http(80),
                    _bitshift(resolution > 4 ? 4 : 8 - resolution) {

                }

                /**
                 * Update histogram
                 *
                 * @param decoder
                 */
                void update(JpegDecoder& decoder) {
                    reset();

                    for (size_t i = 0; i < decoder.luma.length; i++)
                        y[decoder.luma.pixels[i] >> _bitshift] += 1;

                    for (size_t i = 0; i < decoder.cb.length; i++)
                        cb[decoder.cb.pixels[i] >> _bitshift] += 1;

                    for (size_t i = 0; i < decoder.cr.length; i++)
                        cr[decoder.cr.pixels[i] >> _bitshift] += 1;
                }

                /**
                 * Print histogram as JSON
                 *
                 * @tparam Printer
                 * @param printer
                 */
                template<typename Printer>
                void printAsJson(Printer& printer) {
                    printer.print("{\"y\":");
                    printArrayAsJson(printer, y);
                    printer.print(",\"cb\":");
                    printArrayAsJson(printer, cb);
                    printer.print(",\"cr\":");
                    printArrayAsJson(printer, cr);
                    printer.print('}');
                }

                /**
                 *
                 * @return
                 */
                String toJson() {
                    return
                        String("{\"y\":")
                        + toJsonArray(y)
                        + ",\"cb\":"
                        + toJsonArray(cb)
                        + ",\"cr\":"
                        + toJsonArray(cr)
                        + ",\"bins\":"
                        + (256 >> _bitshift)
                        + '}';
                }

                /**
                 * Synctactic sugar
                 *
                 * @return
                 */
                bool httpServe() {
                    return httpServeOn(80);
                }

                /**
                 *
                 * @param port
                 * @return
                 */
                bool httpServeOn(uint8_t port = 80) {
                    http.listenOn(port);

                    http.get("/", [](httpd_req_t *req) {
                        httpd_resp_sendstr(req, ((HistogramYCbCr *) req->user_ctx)->toJson().c_str());

                        return ESP_OK;
                    }, this);

                    if (!http.begin())
                        return setErrorMessage("Cannot start HTTP server");

                    return setErrorMessage("");
                }

            protected:
                uint8_t _bitshift;

                /**
                 * Reset histogram
                 */
                void reset() {
                    for (uint8_t i = 0; i < 16; i++)
                        y[i] = cb[i] = cr[i] = 0;
                }

                /**
                 *
                 * @tparam Printer
                 * @param printer
                 * @param array
                 */
                template<typename Printer>
                void printArrayAsJson(Printer& printer, uint16_t *array) {
                    printer.print('[');
                    printer.print(array[0]);

                    for (uint8_t i = 1, l = (256 >> _bitshift); i < l; i++) {
                        printer.print(',');
                        printer.print(array[i]);
                    }

                    printer.print(']');
                }

                /**
                 *
                 * @param array
                 * @return
                 */
                String toJsonArray(uint16_t *array) {
                    String json;

                    json.reserve(16 * 4 + 2);
                    json += '[';
                    json += array[0];

                    for (uint8_t i = 1, l = (256 >> _bitshift); i < l; i++) {
                        json += ',';
                        json += array[i];
                    }

                    json += ']';

                    return json;
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_HISTOGRAMYCBCR_H
