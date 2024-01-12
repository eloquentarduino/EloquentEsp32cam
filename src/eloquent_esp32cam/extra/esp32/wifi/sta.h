#ifndef ELOQUENT_EXTRA_ESP32_WIFI_WIFISTA
#define ELOQUENT_EXTRA_ESP32_WIFI_WIFISTA

#include <WiFi.h>
#include <ESPmDNS.h>
#include "../../exception.h"

using Eloquent::Error::Exception;


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Wifi {
                /**
                 * Connect to WiFi in Station mode
                 */
                class Sta {
                    public:
                        Exception exception;

                        /**
                         * Constructor
                         */
                        Sta() :
                            exception("WiFi") {

                        }

                        /**
                         * Test if there's an exception
                         */
                        operator bool() const {
                            return isConnected();
                        }

                        #ifdef WIFI_SSID
                        #ifdef WIFI_PASS
                            Exception& connect(size_t timeout = 20000) {
                                return connect(WIFI_SSID, WIFI_PASS, timeout);
                            }
                        #endif
                        #endif

                        /**
                         * Connect to WiFi network
                         */
                        Exception& connect(const char *ssid, const char* password, size_t timeout = 20000) {
                            ESP_LOGI("WiFi", "Connecting to %s...", ssid);
                            WiFi.mode(WIFI_STA);
                            WiFi.disconnect(true, true);
                            WiFi.begin(ssid, password);
                            /*WiFi.persistent(true);
                            WiFi.setAutoConnect(false);
                            WiFi.setAutoReconnect(true);
                            WiFi.setTxPower(WIFI_POWER_2dBm);*/

                            timeout += millis();

                            while (millis() < timeout) {
                                if (isConnected()) {
                                    ESP_LOGI("WiFi", "Connected!");

                                    #ifdef HOSTNAME
                                        MDNS.begin(HOSTNAME);
                                    #endif

                                    return exception.clear();
                                }

                                delay(50);
                            }

                            if (WiFi.status() == 1)
                                return exception.set("SSID not found");

                            return exception.set("Can't connect to WiFi (wrong password?)");
                        }

                        /**
                         * Test if camera is connected to WiFi
                         * @return
                         */
                        inline bool isConnected() const {
                            return WiFi.status() == WL_CONNECTED;
                        }

                        /**
                         * Get IP address as string
                         *
                         * @return
                         */
                        String ip() const {
                            IPAddress ip = WiFi.localIP();

                            return String(ip[0]) + '.' + ip[1] + '.' + ip[2] + '.' + ip[3];
                        }
                };
            }
        }
    }
}

namespace eloq {
    static Eloquent::Extra::Esp32::Wifi::Sta wifi;
}

#endif