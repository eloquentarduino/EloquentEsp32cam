//
// Created by Simone on 11/11/22.
//

#ifndef ELOQUENTESP32CAM_HASMDNS_H
#define ELOQUENTESP32CAM_HASMDNS_H

#include <ESPmDNS.h>


namespace Eloquent {
    namespace Esp32cam {
        /**
         * Add mDNS capabilities to ESP32
         */
        class HasMDNS {
        public:

            /**
             * @deprecated
             * @param hostname
             * @return
             */
            bool viewAt(const char *hostname) {
                return mDNS(hostname);
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
        };
    }
}

#endif //ELOQUENTESP32CAM_HASMDNS_H
