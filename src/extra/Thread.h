//
// Created by Simone on 05/07/23.
//

#ifndef ELOQUENTEXTRA_THREAD_H
#define ELOQUENTEXTRA_THREAD_H

namespace Eloquent {
    namespace Extra {
        /**
         * Run task on given core
         */
        class Thread {
        public:

            /**
             *
             * @param name
             */
            Thread(const char* name, uint16_t stackSize = 1000) :
                _name(name),
                _priority(0),
                _stack(stackSize),
                _args(NULL) {

                _core = xPortGetCoreID();
            }

            /**
             * Set task args
             * @return
             */
            Thread& withArgs(void* args) {
                _args = args;

                return *this;
            }

            /**
             * Set args if not set
             * @return
             */
            Thread& withDefaultArgs(void* args) {
                if (_args == NULL)
                    _args = args;

                return *this;
            }

            /**
             * Set task priority
             * @return
             */
            Thread& withPriority(uint8_t priority) {
                _priority = priority;

                return *this;
            }

            /**
             * Set stack size
             * @return
             */
            Thread& withStack(uint16_t stack) {
                _stack = stack;

                return *this;
            }

            /**
             * Set pinned core
             * @return
             */
            Thread& onCore(uint8_t core) {
                _core = core;

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
                xTaskCreate(
                        task,      // Function to implement the task
                        _name,     // Name of the task
                        _stack,    // Stack size in bytes
                        _args,     // Task input parameter
                        _priority, // Priority of the task
                        NULL      // Task handle.
                        //_core      // Core where the task should run
                );
            }

        private:
            const char *_name;
            uint8_t _core;
            void *_args;
            uint8_t _priority;
            uint16_t _stack;
        };
    }
}

#endif //ELOQUENTEXTRA_THREAD_H
