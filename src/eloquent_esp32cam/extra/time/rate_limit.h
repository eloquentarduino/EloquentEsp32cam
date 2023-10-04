#ifndef ELOQUENT_EXTRA_RATELIMIT
#define ELOQUENT_EXTRA_RATELIMIT

namespace Eloquent {
    namespace Extra {
        namespace Time {

            /**
             * Prevent running code too often
             */
            class RateLimit {
                public:

                    /**
                     * Constructor
                     */
                    RateLimit() :
                        debounceTime(0),
                        lastEvent(0) {

                        }

                    /**
                     * Test if debounce time has elapsed
                     */
                    operator bool() const {
                        const size_t now = millis();

                        return debounceTime == 0 || lastEvent == 0 || (now - lastEvent) >= debounceTime || lastEvent >= now;
                    }

                    /**
                     * Set debounce interval
                     */
                    inline RateLimit& none() {
                        debounceTime = 0;

                        return *this;
                    }

                    /**
                     * Set debounce interval
                     */
                    inline RateLimit& atMostOnceEvery(size_t x) {
                        debounceTime = x;

                        return *this;
                    }

                    /**
                     * Set debounce fps
                     */
                    inline RateLimit& atMost(size_t x) {
                        debounceTime = x;

                        return *this;
                    }

                    /**
                     * Set debounce unit
                     */
                    inline RateLimit& milliseconds() {
                        return *this;
                    }

                    /**
                     * Set debounce unit
                     */
                    inline RateLimit& second() {
                        return seconds();
                    }

                    /**
                     * Set debounce unit
                     */
                    inline RateLimit& seconds() {
                        debounceTime *= 1000;

                        return *this;
                    }

                    /**
                     * Set debounce unit
                     */
                    inline RateLimit& minutes() {
                        debounceTime *= 1000;
                        debounceTime *= 60;

                        return *this;
                    }

                    /**
                     * Set debounce unit
                     */
                    inline RateLimit& hours() {
                        debounceTime *= 1000;
                        debounceTime *= 3600;

                        return *this;
                    }

                    /**
                     * Set debounce unit
                     */
                    inline RateLimit& fps() {
                        debounceTime = 1000 / debounceTime;

                        return *this;
                    }

                    /**
                     * Update last event timestamp
                     */
                    inline void touch() {
                        lastEvent = millis();
                    }

                    /**
                     * Get informative text on when next event is allowed
                     */
                    String getRetryInMessage() {
                        return String("Rate limit exceeded. Retry in ")
                            + (debounceTime - (millis() - lastEvent))
                            + "ms";
                    }

                protected:
                    size_t debounceTime;
                    size_t lastEvent;
            };
        }
    }
}

#endif