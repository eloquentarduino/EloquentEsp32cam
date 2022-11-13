//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_CONNECTSTOWIFI_H
#define ELOQUENTESP32CAM_CONNECTSTOWIFI_H


#include <WiFi.h>

namespace Eloquent {
    namespace Esp32cam {
        /**
         * Helper functions to manage WiFi
         */
        class ConnectsToWiFi {
        public:

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
                    if (WiFi.status() == WL_CONNECTED)
                        return true;

                    delay(100);
                }

                return false;
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
        };
    }
}

#endif //ELOQUENTESP32CAM_CONNECTSTOWIFI_H
