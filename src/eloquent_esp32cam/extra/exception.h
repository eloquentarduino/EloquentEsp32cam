#ifndef ELOQUENT_EXTRA_ERROR_EXCEPTION_H
#define ELOQUENT_EXTRA_ERROR_EXCEPTION_H

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
                Exception(const char* tag) : 
                    _tag(tag), 
                    _message(""),
                    _isSevere(true) {
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
                    return _message == "";
                }

                /**
                 * Test if exception is severe
                 */
                bool isSevere() const {
                    return _isSevere && !isOk();
                }

                /**
                 * Mark error as not severe
                 */
                Exception& soft() {
                    _isSevere = false;

                    return *this;
                }

                /**
                 * Set exception message
                 */
                Exception& set(String error) {
                    _message = error;
                    _isSevere = true;

                    if (error.length() > 0) {
                        const char *c_str = error.c_str();
                        ESP_LOGE(_tag, "%s", c_str);
                    }

                    return *this;
                }

                /**
                 * Clear exception
                 */
                Exception& clear() {
                    return set("");
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
                inline String toString() {
                    return _message;
                }

                /**
                 * Convert exception to char*
                 */
                inline const char* toCString() {
                    return toString().c_str();
                }

                /**
                 * 
                 */
                static Exception none() {
                    return Exception("");
                }

            protected:
                const char* _tag;
                bool _isSevere;
                String _message;
        };
    }
}

#endif