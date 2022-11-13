//
// Created by Simone on 13/11/22.
//

#ifndef ELOQUENTESP32CAM_NTPCLIENT_H
#define ELOQUENTESP32CAM_NTPCLIENT_H

namespace Eloquent {
    namespace Esp32cam {

        /**
         * Get time from NTP server
         */
        class NtpClient {
        public:
            struct tm timeinfo;

            /**
             *
             */
            NtpClient() :
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
             *
             */
            inline void westernEurope() {
                gmt(0);
            }

            /**
             *
             */
            inline void centralEurope() {
                gmt(1);
            }

            /**
             *
             */
            inline void easternEurope() {
                gmt(2);
            }

            /**
             * Set daylight offset
             */
            inline void daylight() {
                _daylightOffset = 1;
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
                configTime(_gmtOffset * 3600, _daylightOffset * 3600, (const char*) _server.c_str());

                return refresh();
            }

            /**
             *
             * @param prefix
             * @return
             */
            String getFilename(String prefix = "/") {
                return getDateTimeFilename(prefix);
            }

            /**
             *
             * @param prefix
             * @return
             */
            String getDateFilename(String prefix = "/") {
                return getFilename(prefix, "%Y-%m-%d");
            }

            /**
             *
             * @param prefix
             * @return
             */
            String getDateTimeFilename(String prefix = "/") {
                return getFilename(prefix, "%Y-%m-%d_%H-%M-%S");
            }

            /**
             *
             * @param prefix
             * @param fmt
             * @return
             */
            String getFilename(String prefix, const char *fmt) {
                char buf[64];

                if(!refresh())
                    return prefix + millis() + ".jpg";

                strftime(buf, sizeof(buf), fmt, &timeinfo);

                return prefix + buf + ".jpg";
            }

        protected:
            uint8_t _gmtOffset;
            uint8_t _daylightOffset;
            String _server;

            /**
             * Update time
             * @return
             */
            bool refresh() {
                getLocalTime(&timeinfo);

                return timeinfo.tm_year >= (2022 - 1900);
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_NTPCLIENT_H
