//
// Created by Simone on 05/11/22.
//

#ifndef ELOQUENTESP32CAM_CONNECTSTOWIFI_H
#define ELOQUENTESP32CAM_CONNECTSTOWIFI_H


#include <WiFi.h>
#include <ESPmDNS.h>
#include "../../traits/HasErrorMessage.h"

#ifndef HOSTNAME
#define HOSTNAME "esp32cam"
#endif


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Helper functions to manage WiFi
             */
            class ConnectsToWiFi : public HasErrorMessage {
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
                 * Set connection timeout in seconds
                 *
                 * @param timeout
                 */
                void setConnectionTimeout(size_t timeout) {
                    _connectionTimeout = timeout * 1000;
                }

                /**
                 * Autoconnect to WiFi on begin()
                 * @return
                 */
                bool autoconnect() {
                    if (!_shouldAutoconnect)
                        return clearError();

#ifdef WIFI_SSID
#ifdef WIFI_PASS
                    if (WIFI_SSID != "")
                        return connect(WIFI_SSID, WIFI_PASS);
#endif
#endif

                    return clearError();
                }

                /**
                 * Connect to WiFi network as client
                 *
                 * @param ssid
                 * @param password
                 * @param timeout
                 * @return
                 */
                bool connect(const char *ssid, const char *password, uint16_t timeout = 0) {
                    WiFi.mode(WIFI_STA);
                    WiFi.begin(ssid, password);

                    uint16_t start = millis();

                    if (!timeout)
                        timeout = _connectionTimeout;

                    while (millis() - start < timeout) {
                        if (isConnected()) {
                            mDNS(HOSTNAME);
                            return setErrorMessage("");
                        }

                        delay(100);
                    }

                    return setErrorMessage("Can't connect to WiFi", "WiFi");
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
                inline bool isConnected() {
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
                size_t _connectionTimeout = 20000UL;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_CONNECTSTOWIFI_H
