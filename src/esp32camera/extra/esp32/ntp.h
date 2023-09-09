#ifndef ELOQUENT_EXTRA_ESP32_NTP
#define ELOQUENT_EXTRA_ESP32_NTP

#include <WiFi.h>
#include "../error/Exception.h"

using Eloquent::Extra::Error::Exception;

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            class NTP {
                public:
                    struct tm timeinfo;
                    Exception exception;

                    /**
                     * 
                     */
                    NTP() :
                        exception("NTP"),
                        gmtOffset(0),
                        daylightOffset(0),
                        serverName("pool.ntp.org") {

                        }

                    /**
                     * Test if NTP is working
                     */
                    operator bool() const {
                        return timeinfo.tm_year >= (2023 - 1900);
                    }

                    /**
                     * Short for offsetFromGreenwhich
                     */
                    void gmt(float offset = 0) {
                        offsetFromGreenwhich(offset);
                    }

                    /**
                     * 
                     */
                    void offsetFromGreenwhich(float offset) {
                        gmtOffset = offset * 3600;
                    }

                    /**
                     * 
                     */
                    void isDaylight(bool is = true) {
                        daylightOffset = is ? 3600 : 0;
                    }

                    /**
                     * 
                     */
                    void isntDaylight() {
                        isDaylight(false);
                    }

                    /**
                     * 
                     */
                    void server(String server) {
                        serverName = server;
                    }

                    /**
                     * Configure NTP
                     */
                    Exception& begin() {
                        if (WiFi.status() != WL_CONNECTED)
                            return exception.set("You need WiFi connection to sync NTP");

                        configTime(gmtOffset, daylightOffset, (const char *) serverName.c_str());
                    
                        return refresh();
                    }

                    /**
                     * Update time
                     */
                    Exception& refresh() {
                        getLocalTime(&timeinfo);

                        if (timeinfo.tm_year < (2023 - 1900))
                            return exception.set("Cannot get time");

                        return exception.clear();
                    }

                    /**
                     * Format datetime
                     */
                    String format(const char *fmt) {
                        char buf[32];

                        if (!!exception)
                            return "";

                        strftime(buf, sizeof(buf), fmt, &timeinfo);

                        return buf;
                    }

                    /**
                     * Get date as string
                     */
                    String date() {
                        return format("%d-%m-%Y");
                    }

                    /**
                     * Get datetime as string
                     */
                    String datetime() {
                        return format("%d-%m-%YT%H:%M:%S");
                    }

                    /**
                     * Get datetime as string
                     */
                    String filenameDatetime() {
                        return format("%Y%m%d_%H%M%S");
                    }

                    // synctactic sugar
                    inline void cst() { gmt(8); }
                    inline void ist() { gmt(5.5); }
                    inline void eest() { gmt(3); }
                    inline void cest() { gmt(2); }
                    inline void bst() { gmt(1); }
                    inline void west() { gmt(1); }
                    inline void cet() { gmt(1); }
                    inline void edt() { gmt(-4); }
                    inline void pdt() { gmt(-7); }

                protected:
                    uint16_t gmtOffset;
                    uint16_t daylightOffset;
                    String serverName;
                    
            };
        }
    }
}

namespace e {
    static Eloquent::Extra::Esp32::NTP ntp;
}

#endif