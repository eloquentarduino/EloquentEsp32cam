#ifndef ELOQUENT_EXTRA_ERROR_EXCEPTION
#define ELOQUENT_EXTRA_ERROR_EXCEPTION

namespace Eloquent {
    namespace Extra {
        /**
         * Application expcetion
         */
        class Exception {
            public:
                /**
                 * 
                 */
                Exception(const char* tag) {
                    this->tag = tag;
                    this->message = "";
                }

                /**
                 * Test if there's an exception
                 */
                operator bool() const {
                    return !isOk();
                }

                /**
                 * Test if there's an exception
                 */
                bool isOk() const {
                    return message == "";
                }

                /**
                 * Set exception message
                 */
                Exception& set(String error) {
                    this->message = error;

                    if (error.length() > 0)
                        //ESP_LOGE(tag, error);
                        log_e("[%s] %s", tag, error.c_str());

                    return *this;
                }

                /**
                 * Clear exception
                 */
                Exception& clear() {
                    this->message = "";

                    return *this;
                }

                /**
                 * 
                 */
                template<typename Other>
                Exception& propagate(Other& other) {
                    set(other.exception.toString());

                    return *this;
                }

                /**
                 * Convert exception to string
                 */
                String toString() {
                    return message;
                }

                /**
                 * 
                 */
                static Exception none() {
                    return Exception("");
                }

            protected:
                const char* tag;
                String message;
        };
    }
}

#endif