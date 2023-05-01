//
// Created by Simone on 13/11/22.
//

#ifndef ELOQUENTESP32CAM_SYNCS_TIME_H
#define ELOQUENTESP32CAM_SYNCS_TIME_H

#include "../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {

            /**
             * Get time from NTP server
             */
             template<typename Camera>
            class SyncsTime : public HasErrorMessage {
            public:
                struct tm timeinfo;

                /**
                 *
                 */
                SyncsTime(Camera *camera) :
                    cam(camera),
                    _gmtOffset(0),
                    _daylightOffset(0),
                    _server("pool.ntp.org") {

                }

                /**
                 * Set GMT offset
                 *
                 * @param gmtOffsetInHours
                 */
                inline void gmt(uint8_t gmtOffsetInHours) {
                    _gmtOffset = gmtOffsetInHours;
                }

                /**
                 * Short for gmt + 0
                 */
                inline void westernEurope() {
                    gmt(0);
                }

                /**
                 * Short for gmt + 1
                 */
                inline void centralEurope() {
                    gmt(1);
                }

                /**
                 * Short for gmt + 2
                 */
                inline void easternEurope() {
                    gmt(2);
                }

                /**
                 * Set daylight offset
                 *
                 * @param enabled
                 */
                inline void daylight(bool enabled = true) {
                    _daylightOffset = enabled ? 1 : 0;
                }

                /**
                 * Set server address
                 *
                 * @param server
                 */
                inline void server(String server) {
                    _server = server;
                }

                /**
                 * Initialize
                 */
                bool begin() {
                    if (!cam->wifi.isConnected())
                        return setErrorMessage("WiFi not connected, can't sync time", "NTP");

                    configTime(_gmtOffset * 3600, _daylightOffset * 3600, (const char *) _server.c_str());

                    return refresh();
                }

                /**
                 * Get date as string
                 * @return
                 */
                String getDate() {
                    return format("%Y%m%d");
                }

                /**
                 * Get datetime as string
                 * @return
                 */
                String getDateTime() {
                    return format("%Y%m%d_%H%M%S");
                }

                /**
                 * Format datetime
                 * 
                 * @param fmt
                 * @return
                 */
                String format(const char *fmt) {
                    char buf[32];

                    if (!refresh())
                        return "";

                    strftime(buf, sizeof(buf), fmt, &timeinfo);

                    return buf;
                }

                /**
                 * Update time
                 * 
                 * @return
                 */
                bool refresh() {
                    getLocalTime(&timeinfo);

                    return timeinfo.tm_year >= (2022 - 1900);
                }

            protected:
                uint8_t _gmtOffset;
                uint8_t _daylightOffset;
                String _server;
                Camera *cam;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SYNCS_TIME_H
