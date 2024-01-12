#ifndef ELOQUENT_ESP32CAM_VIZ_MOTION_ROI_DETECTION
#define ELOQUENT_ESP32CAM_VIZ_MOTION_ROI_DETECTION

#include "../../extra/exception.h"
#include "../../extra/esp32/wifi/sta.h"
#include "../../extra/esp32/http/server.h"
#include "../../motion/roi_detection.h"
#include "../mjpeg.h"

using namespace eloq;
using Eloquent::Error::Exception;
using Eloquent::Extra::Esp32::Http::HttpServer;
using Eloquent::Esp32cam::Motion::RoI;


namespace Eloquent {
    namespace Esp32cam {
        namespace Viz {
            namespace Motion {
                /**
                 * Visualize RoI motion detection
                 */
                class RoIMotionDetection {
                    public:
                        Exception exception;
                        HttpServer server;

                        /**
                         * 
                         */
                        RoIMotionDetection() :
                            _head(0),
                            exception("RoIVizServer"),
                            server("RoIVizServer") {

                        }

                        /**
                         * Add roi to viz
                         */
                        Exception& add(RoI &roi) {
                            if (_head >= 10)
                                return exception.set("Too many rois (10 max)");

                            _rois[_head++] = &roi;

                            return exception.clear();
                        }

                        /**
                         * Debug self IP address
                         */
                        String address() const {
                            return String("RoI Viz is available at http://") + wifi.ip();
                        }

                        /**
                         * Start server
                         */
                        Exception& begin() {
                            if (!wifi.isConnected())
                                return exception.set("WiFi not connected");

                            // start mjpeg http server
                            if (!mjpeg.begin().isOk())
                                return exception.propagate(mjpeg);

                            onIndex();
                            onRoIs();

                            // run in thread
                            server.thread.withStackSize(7000);

                            if (!server.begin().isOk())
                                return exception.propagate(server);

                            return exception.clear();
                        }


                    protected:
                        uint8_t _head;
                        RoI *_rois[10];

                        /**
                         * Register / endpoint to get main page
                         */
                        void onIndex() {
                            server.onGET("/", [this](WebServer *web) {
                                web->send(200, "text/plain", "RoI Viz Server");
                            });
                        }

                        /**
                         * Register / endpoint to get main page
                         */
                        void onRoIs() {
                            server.onGET("/rois", [this](WebServer *web) {
                                for (int i = 0; i < _head; i++) {
                                    RoI *roi = _rois[i];

                                    roi->updateCoords(camera.resolution.getWidth(), camera.resolution.getHeight());

                                    web->sendContent("{\"x1\":");
                                    web->sendContent(String(roi->coords.x1));
                                    web->sendContent(",\"x2\":");
                                    web->sendContent(String(roi->coords.x2));
                                    web->sendContent(",\"y1\":");
                                    web->sendContent(String(roi->coords.y1));
                                    web->sendContent(",\"y2\":");
                                    web->sendContent(String(roi->coords.y2));  
                                    web->sendContent("}\n");                     
                                }

                                web->sendContent("");
                            });
                        }
                };
            }
        }
    }
}

namespace eloq {
    static Eloquent::Esp32cam::Viz::Motion::RoIMotionDetection roiViz;
}

#endif