#ifdef ESP32
#ifndef ELOQUENT_EXTRA_ESP32_MULTIPROCESSING_MUTEX
#define ELOQUENT_EXTRA_ESP32_MULTIPROCESSING_MUTEX

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Multiprocessing {
                /**
                 * Mutex for concurrent access to resource
                 */
                class Mutex {
                    public:
                        SemaphoreHandle_t mutex;
                        
                        /**
                         * 
                         */
                        Mutex() {
                            mutex = xSemaphoreCreateMutex();
                        }

                        /**
                         * Run function with mutex
                         */
                        template<typename Callback>
                        bool threadsafe(Callback callback, size_t timeout = portMAX_DELAY) {
                            if (xSemaphoreTake(mutex, timeout) != pdTRUE)
                                return false;

                            callback();
                            xSemaphoreGive(mutex);

                            return true;
                        }
                };
            }
        }
    }
}

#endif
#endif