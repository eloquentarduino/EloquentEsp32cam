#ifndef ELOQUENT_EXTRA_ESP32_NVS_COUNTER
#define ELOQUENT_EXTRA_ESP32_NVS_COUNTER

#include <Preferences.h>
#include "../../ulid.h"

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace NVS {
                /**
                 * Keep incremental counter in NVS
                 */
                class Counter {
                    public:

                        /**
                         * 
                         */
                        Counter(const char *name) : 
                            key(name),
                            count(0) {
                        }

                        /**
                         * Read current value from NVS
                         */
                        unsigned long current() {
                            prefs.begin("e::counter", true);
                            count = prefs.getULong(key, 0);
                            prefs.end();

                            return count;
                        }

                        /**
                         * Increment counter
                         */
                        unsigned long next() {
                            count = current();
                            count += 1;

                            set(count);

                            return count;
                        }

                        /**
                         * 
                         */
                        void set(uint16_t n) {
                            prefs.begin("e::counter", false);
                            prefs.putULong(key, count);
                            prefs.end();
                        }

                        /**
                         * 
                         */
                        void reset() {
                            set(0);
                        }

                        /**
                         * Get current count as padded string
                         */
                        String toString(uint8_t length = 8) {
                            readIfNecessary();

                            String s = String(count);

                            while (s.length() < length)
                                s = String("0") + s;

                            return s;
                        }

                        /**
                         * Get next count as padded string
                         */
                        String nextString(uint8_t length = 8) {
                            next();

                            return toString(length);
                        }

                        /**
                         * Get current count as Ulid
                         */
                        String toUlid() {
                            readIfNecessary();

                            ULID ulid(count, count % 255);

                            return ulid.toString();
                        }

                        /**
                         * Get next count as Ulid
                         */
                        String nextUlid() {
                            next();

                            return toUlid();
                        }

                    protected:
                        unsigned long count;
                        const char *key;
                        Preferences prefs;

                        /**
                         * 
                         */
                        unsigned long readIfNecessary() {
                            return count > 0 ? count : current();
                        }
                };
            }
        }
    }
}

#endif