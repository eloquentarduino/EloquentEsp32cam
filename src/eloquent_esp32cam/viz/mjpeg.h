#ifndef ELOQUENT_ESP32CAM_VIZ_MJPEG
#define ELOQUENT_ESP32CAM_VIZ_MJPEG

#include "../camera/camera.h"
#include "../extra/exception.h"
#include "../extra/esp32/wifi/sta.h"
#include "../extra/esp32/http/server.h"

using eloq::camera;
using eloq::wifi;
using Eloquent::Error::Exception;
using Eloquent::Extra::Esp32::Http::HttpServer;
using OnFrameCallback = std::function<void(WiFiClient*, camera_fb_t*)>;

namespace Eloquent {
    namespace Esp32cam {
        namespace Viz {
            /**
             * Stream MJPEG video to browser
             */
            class Mjpeg {
                public:
                    Exception exception;
                    HttpServer server;

                    /**
                     * Constructor
                     */
                    Mjpeg() :
                        exception("Mjpeg"),
                        server("Mjpeg", MJPEG_HTTP_PORT),
                        _paused(false),
                        _stopped(false),
                        _delay(50),
                        _onFrame(NULL) {

                        }

                    /**
                     * Debug self IP address
                     */
                    String address() const {
                        return String("MJPEG stream is available at http://") + wifi.ip() + ":" + String(MJPEG_HTTP_PORT);
                    }

                    /**
                     * Limit FPS
                     * @param fps
                     */
                    void maxFPS(uint8_t fps) {
                        _delay = 1000.0f / fps;

                        if (_delay < 1)
                            _delay = 1;
                    }

                    /**
                     * Start server
                     */
                    Exception& begin() {
                        onJpeg();
                        onMjpeg();
                        onHtml();

                        // run in thread
                        server.thread.withStackSize(5000);

                        if (!server.begin().isOk())
                            return exception.propagate(server);

                        return exception.clear();
                    }

                    /**
                     * Pause stream
                     */
                    void pause() {
                        _paused = true;
                    }

                    /**
                     * Play stream
                     */
                    void play() {
                        _paused = false;
                        _stopped = false;
                    }

                    /**
                     * Completely stop the stream
                     */
                    void stop() {
                        _stopped = true;
                    }

                    /**
                     * Set callback to run on each streamed frame
                     * @param onFrame
                     */
                    void onFrame(OnFrameCallback onFrame) {
                        _onFrame = onFrame;
                    }

                protected:
                    bool _paused;
                    bool _stopped;
                    uint16_t _delay;
                    OnFrameCallback _onFrame;

                    /**
                     * Register / endpoint to get Mjpeg stream
                     */
                    void onMjpeg() {
                        server.onGET("/", [this](WebServer *web) {
                            if (_stopped) {
                                web->send(500, "text/plain", "Server is stopped");
                                return;
                            }

                            WiFiClient client = web->client();

                            client.println(F("HTTP/1.1 200 OK"));
                            client.println(F("Content-Type: multipart/x-mixed-replace;boundary=frame"));
                            client.println(F("Access-Control-Allow-Origin: *"));
                            client.println(F("\r\n--frame"));

                            size_t last = millis();

                            while (true) {
                                while (millis() - last < _delay)
                                    delay(1);

                                yield();

                                if (_paused)
                                    continue;

                                if (_stopped)
                                    break;

                                if (!client.connected())
                                    break;

                                if (!camera.capture().isOk())
                                    continue;

                                if (_onFrame != NULL)
                                    _onFrame(&client, camera.frame);

                                client.print("Content-Type: image/jpeg\r\nContent-Length: ");
                                client.println(camera.frame->len);
                                client.println();
                                client.write((const char *) camera.frame->buf, camera.frame->len);
                                client.println(F("\r\n--frame"));
                                client.flush();
                                last = millis();
                            }
                        });
                    }

                    /**
                     * Register /jpeg endpoint to get a single Jpeg
                     */
                    void onJpeg() {
                        server.onGET("/jpeg", [this](WebServer *web) {
                            if (_stopped) {
                                web->send(500, "text/plain", "Server is stopped");
                                return;
                            }

                            if (!camera.capture().isOk())
                                return server.serverError(camera.exception.toString());

                            WiFiClient client = web->client();

                            client.println(F("HTTP/1.1 200 OK"));
                            client.println(F("Content-Type: image/jpeg"));
                            client.println(F("Access-Control-Allow-Origin: *"));
                            client.print(F("Content-Length: "));
                            client.println((unsigned int) camera.frame->len);
                            client.println();
                            client.write((const char *) camera.frame->buf, camera.frame->len);
                        });
                    }

                    /**
                     * Register /html endpoint to get a full HTML page
                     */
                    void onHtml() {
                        server.onGET("/html", [this](WebServer *web) {
                            if (_stopped) {
                                web->send(500, "text/plain", "Server is stopped");
                                return;
                            }
                            
                            String html = String("<img src=\"http://") + wifi.ip() + ":" + String(MJPEG_HTTP_PORT) + "\" />";
                            web->send(200, "text/html", html);
                        });
                    }
            };
        }
    }
}

namespace eloq {
    namespace viz {
        static Eloquent::Esp32cam::Viz::Mjpeg mjpeg;
    }
}

#endif