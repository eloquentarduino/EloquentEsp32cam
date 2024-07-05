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
                        const char *name;
                        SemaphoreHandle_t mutex;
                        
                        /**
                         * 
                         */
                        Mutex(const char *name_) :
                            mutex(NULL),
                            name(name_),
                            _ok(true) {
                        }

                        /**
                         * 
                         */
                        bool isOk() {
                            return _ok;
                        }

                        /**
                         * Run function with mutex
                         */
                        template<typename Callback>
                        bool threadsafe(Callback callback, size_t timeout = 0) {
                            portTickType ticks = timeout / portTICK_RATE_MS;

                            if (ticks == 0)
                                ticks = portMAX_DELAY;

                            if (mutex == NULL) {
                                ESP_LOGI("Mutex", "Creating mutex %s", name);
                                mutex = xSemaphoreCreateMutex();
                            }

                            if (mutex == NULL) {
                                ESP_LOGE("Mutex", "Cannot create mutex %s", name);
                                return (_ok = false);
                            }

                            if (xSemaphoreTake(mutex, ticks) != pdTRUE) {
                                ESP_LOGW("Mutex", "Cannot acquire mutex %s within timeout", name);
                                return (_ok = false);
                            }

                            callback();
                            xSemaphoreGive(mutex);

                            return (_ok = true);
                        }

                    protected:
                        bool _ok;
                };
            }
        }
    }
}

#endif
#endif