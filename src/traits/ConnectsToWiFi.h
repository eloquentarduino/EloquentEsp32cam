//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_CONNECTSTOWIFI_H
#define ELOQUENTESP32CAM_CONNECTSTOWIFI_H


#include <WiFi.h>
#include <ESPmDNS.h>
#include "../esp32cam/Cam.h"


namespace Eloquent {
    namespace Esp32cam {
        /**
         * Helper functions to manage WiFi
         */
        class ConnectsToWiFi {
        public:

            /**
             * Turn on automatic connect on begin()
             */
            void shouldAutoconnect() {
                _shouldAutoconnect = true;
            }

            /**
             * Turn off automatic connect on begin()
             */
            void shouldntAutoconnect() {
                _shouldAutoconnect = false;
            }

            /**
             * Autoconnect to WiFi on begin()
             * @return
             */
            bool autoconnect() {
                if (!_shouldAutoconnect)
                    return true;

#ifdef WIFI_SSID
#ifdef WIFI_PASS
                return connect(WIFI_SSID, WIFI_PASS);
#endif
#endif

                return true;
            }

            /**
             * Connect to WiFi network as client
             *
             * @param ssid
             * @param password
             * @param timeout
             * @return
             */
            bool connect(const char *ssid, const char *password, uint16_t timeout = 20000) {
                WiFi.mode(WIFI_STA);
                WiFi.begin(ssid, password);

                uint16_t start = millis();

                while (millis() - start < timeout) {
                    if (isConnectedToWiFi()) {
                        mDNS("esp32cam");
                        return true;
                    }

                    delay(100);
                }

                return false;
            }

            /**
             * Set mDNS hostname
             *
             * @param hostname
             * @return
             */
            bool mDNS(const char *hostname) {
                return MDNS.begin(hostname);
            }

            /**
             * Test if camera is connected to WiFi
             * @return
             */
            inline bool isConnectedToWiFi() {
                return WiFi.status() == WL_CONNECTED;
            }

            /**
             * Get IP address as string
             *
             * @return
             */
            String getIP() {
                IPAddress ip = WiFi.localIP();

                return String(ip[0]) + '.' + ip[1] + '.' + ip[2] + '.' + ip[3];
            }

        protected:
            bool _shouldAutoconnect = true;
        };
    }
}

#endif //ELOQUENTESP32CAM_CONNECTSTOWIFI_H
