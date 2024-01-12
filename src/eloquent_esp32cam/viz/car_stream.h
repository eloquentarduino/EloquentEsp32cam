#ifndef ELOQUENT_ESP32CAM_VIZ_CAR_STREAM_H
#define ELOQUENT_ESP32CAM_VIZ_CAR_STREAM_H

#include "../extra/exception.h"
#include "../extra/esp32/http/server.h"
#include "../extra/car/car2wd.h"
#include "./mjpeg.h"

using namespace eloq;
using Eloquent::Error::Exception;
using Eloquent::Extra::Esp32::Http::HttpServer;
using Eloquent::Extra::Car::Car2WD;


namespace Eloquent {
    namespace Esp32cam {
        namespace Viz {
            /**
             * Display stream of camera
             * with car controls
             */
            class CarStreamServer {
                public:
                    Exception exception;
                    HttpServer server;
                    Car2WD *car;

                    /**
                     * 
                     */
                    CarStreamServer() :
                        exception("CarStreamServer"),
                        server("CarStreamServer") {

                        }

                    /**
                     * Debug self IP address
                     */
                    String address() const {
                        return String("Car stream is available at http://") + wifi.ip();
                    }

                    /**
                     * Assign car
                     */
                    void bind(Car2WD& car) {
                        this->car = &car;
                    }

                    /**
                     * Start server
                     */
                    Exception& begin() {
                        if (!viz::mjpeg.begin().isOk())
                            return exception.propagate(viz::mjpeg);

                        onIndex();
                        onCommand();

                        return server.beginInThread(exception);
                    }

                protected:

                    /**
                     * Display main page
                     */
                    void onIndex() {
                        
                    }

                    /**
                     * Respond to commands over HTTP
                     */
                    void onCommand() {
                        server.onGET("cmd", [this](WebServer *web) {
                            String cmd = web->arg("cmd");

                            if (cmd == "fwd")
                                car->forward();
                            else if (cmd == "back")
                                car->backward();
                            else if (cmd == "left")
                                car->left();
                            else if (cmd == "right")
                                car->right();
                            else
                                car->stop();
                        });
                    }
            };
        }
    }
}


namespace eloq {
    namespace viz {
        static Eloquent::Esp32cam::Viz::CarStreamServer carStream;
    }
}

#endif