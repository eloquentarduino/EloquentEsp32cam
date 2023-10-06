#ifndef ELOQUENT_EXTRA_ESP32_MULTIPROCESSING_THREAD
#define ELOQUENT_EXTRA_ESP32_MULTIPROCESSING_THREAD

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Multiprocessing {
                /**
                 * Run task on given core
                 */
                class Thread {
                public:

                    /**
                     *
                     * @param name
                     */
                    Thread(const char* threadName) :
                        name(threadName),
                        priority(0),
                        stackSize(1000),
                        args(NULL) {

                        core = xPortGetCoreID();
                    }

                    /**
                     * Set task args
                     * @return
                     */
                    Thread& withArgs(void* newArgs) {
                        args = newArgs;

                        return *this;
                    }

                    /**
                     * Set args if not set
                     * @return
                     */
                    Thread& withDefaultArgs(void* defaultArgs) {
                        if (args == NULL)
                            args = defaultArgs;

                        return *this;
                    }

                    /**
                     * Set task priority
                     * @return
                     */
                    Thread& withPriority(uint8_t priority) {
                        this->priority = priority;

                        return *this;
                    }

                    /**
                     * Set stack size
                     * @return
                     */
                    Thread& withStackSize(uint16_t stackSize) {
                        this->stackSize = stackSize;

                        return *this;
                    }

                    /**
                     * Set pinned core
                     * @return
                     */
                    Thread& onCore(uint8_t core) {
                        this->core = core;

                        return *this;
                    }

                    /**
                     * Create task
                     * @tparam Task
                     * @param name
                     * @param task
                     */
                    template<typename Task>
                    void run(Task task) {
                        ESP_LOGI(name, "Starting thread with stack size %d bytes on core %d", (int) stackSize, (int) core);

                        xTaskCreate(
                            task,      // Function to implement the task
                            name,      // Name of the task
                            stackSize, // Stack size in bytes
                            args,      // Task input parameter
                            priority,  // Priority of the task
                            NULL       // Task handle.
                        );
                    }

                private:
                    const char *name;
                    uint8_t core;
                    void *args;
                    uint8_t priority;
                    uint16_t stackSize;
                };
            }
        }
    }
}

#endif
