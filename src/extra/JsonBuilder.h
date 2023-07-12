#pragma once

namespace Eloquent {
    namespace Extra {
        /**
         * Utility class for improved JSON generation
         */
        class JsonBuilder {
            public:

                /**
                 * 
                 */
                JsonBuilder(uint16_t reserve = 0) :
                    isFirstAttribute(true) {
                    if (reserve)
                        json.reserve(reserve);
                }

                /**
                 * Open object literal
                 */
                void object() {
                    json += '{';
                }

                /**
                 * 
                 */
                void closeObject() {
                    if (json.endsWith(",")) {
                        json = json.substring(0, -1);
                    }

                    json += '}';
                }

                /**
                 * Add object prop name
                 */
                void prop(String key) {
                    if (!isFirstAttribute)
                        json += ',';

                    json += '"';
                    json += key;
                    json += "\":";
                }

                /**
                 * 
                 */
                void val(char * value) {
                    val(String(value));
                }

                /**
                 * 
                 */
                void val(String value) {
                    json += '"';
                    json += value;
                    json += '"';
                }

                /**
                 * 
                 */
                template<typename T>
                void val(T value) {
                    json += String(value);
                }

                /**
                 * 
                 */
                template<typename T>
                void add(String key, T value) {
                    prop(key);
                    val(value);
                    isFirstAttribute = false;
                }

                /**
                 * Append raw char
                 */
                template<typename T>
                void raw(T raw) {
                    json += String(raw);
                }

                /**
                 * Append raw string
                 */
                void raw(String raw) {
                    json += raw;
                }

                String& toString() {
                    return json;
                }

            protected:
                String json;
                bool isFirstAttribute;
        };
    }
}