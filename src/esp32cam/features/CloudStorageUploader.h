#pragma once

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Upload picture to https://esp32camstorage.eloquentarduino.com
             */
            class CloudStorageUploader {
            public:
                WiFiClient client;
                const char* ESP32CAMSTORAGE_ROOT_CA PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                                      "MIIFUzCCBDugAwIBAgISA2W3b7EmxaDnw98oQI6oYWcBMA0GCSqGSIb3DQEBCwUA\n"
                                                      "MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n"
                                                      "EwJSMzAeFw0yMzAzMTIxMjE3MDJaFw0yMzA2MTAxMjE3MDFaMDIxMDAuBgNVBAMT\n"
                                                      "J2FwaS5lc3AzMmNhbXN0b3JhZ2UuZWxvcXVlbnRhcmR1aW5vLmNvbTCCASIwDQYJ\n"
                                                      "KoZIhvcNAQEBBQADggEPADCCAQoCggEBAJ+WUA5y0eDoJiz9JGH8V1PGsk/go7Nb\n"
                                                      "ADn9VAtFeLqMPFE9rNVRunjT6xGWJj6RbdZgJd2IQ4ackv/Kbxs+ZfoqH+49q2VC\n"
                                                      "lkoWWOGxnl++PYSP8Ic/MCM/ccW/EdmSBwWcMYr8ciqV0jtWDNppBsBQ8DBe/OQ/\n"
                                                      "i7cYyA3dHsfyDQ3WzE2AGje5t/ImLcjGPI/fE1leqFfS0lDyYV4krrppLt7+0keM\n"
                                                      "vqga+VgsFwOhDTFL6iJZ795nolJ+XNm0FvDgnYL+i95OOtI+3/h7A49ismF4eIS2\n"
                                                      "Vnn5yI89U9ZYK3SQkjPQS/q07ySs84YETXr1EjLnjZzRutv9H5eq6JkCAwEAAaOC\n"
                                                      "AmEwggJdMA4GA1UdDwEB/wQEAwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYB\n"
                                                      "BQUHAwIwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQUMd2bM3oSXD9RX8woXkL6sRrZ\n"
                                                      "zmgwHwYDVR0jBBgwFoAUFC6zF7dYVsuuUAlA5h+vnYsUwsYwVQYIKwYBBQUHAQEE\n"
                                                      "STBHMCEGCCsGAQUFBzABhhVodHRwOi8vcjMuby5sZW5jci5vcmcwIgYIKwYBBQUH\n"
                                                      "MAKGFmh0dHA6Ly9yMy5pLmxlbmNyLm9yZy8wMgYDVR0RBCswKYInYXBpLmVzcDMy\n"
                                                      "Y2Ftc3RvcmFnZS5lbG9xdWVudGFyZHVpbm8uY29tMEwGA1UdIARFMEMwCAYGZ4EM\n"
                                                      "AQIBMDcGCysGAQQBgt8TAQEBMCgwJgYIKwYBBQUHAgEWGmh0dHA6Ly9jcHMubGV0\n"
                                                      "c2VuY3J5cHQub3JnMIIBAwYKKwYBBAHWeQIEAgSB9ASB8QDvAHUAejKMVNi3LbYg\n"
                                                      "6jjgUh7phBZwMhOFTTvSK8E6V6NS61IAAAGG1femcwAABAMARjBEAiBe6V8afFk4\n"
                                                      "cEbF1+10sXLLqCyvd3nCOD5MleaPsHqiQgIgTkDbMvEhGOliz9wR5ksYh7SvDqrB\n"
                                                      "XdoSaNbtrAoOtyQAdgCt9776fP8QyIudPZwePhhqtGcpXc+xDCTKhYY069yCigAA\n"
                                                      "AYbV96Z7AAAEAwBHMEUCIFXr7et0YViH3VAxSUuEgIqIew+j28YcpmjzU5BzT/pO\n"
                                                      "AiEA+Rky/hy4NGa0zdLvhTuJA1WTadErK/Uw7aV2DHJ417owDQYJKoZIhvcNAQEL\n"
                                                      "BQADggEBACVEhaqVk+GJubqT//TRpzKOR+454Lon/6d5WAbGm7BaMXv+z0X8ZM3K\n"
                                                      "YPoLKWDiz3XUvAwosiUW1kUidW0CClabbi0E5s+z1U1wYKWLRdKVFvGHjA2JeyOX\n"
                                                      "qo+SiTjLOS4ZrRoD+yvEG0UJUYAoMqijaiySFzXZZUVAIk4xMAme2x/R6PGcF8wg\n"
                                                      "1TuaCcR7TJcCqxPFK7cohXrUKS3neT+3KeleqfHSGZtDPAdsPDKxj6a1/UIvRNU9\n"
                                                      "jFGr23KkYJ7DTRO3IYoqf2GpdmqADSN13z0KazWaAbQrzJ2j9ueWInYu2MrGTREE\n"
                                                      "7ElyQ1cFZtrZQWjv5GBoYRhnEcPdccg=\n"
                                                      "-----END CERTIFICATE-----";

                /**
                 *
                 * @param deviceToken
                 * @param jpeg
                 * @param size
                 * @return
                 */
                String upload(String deviceToken, uint8_t *jpeg, size_t size) {
                    HTTPClient http;

                    http.begin(String("http://api.esp32camstorage.eloquentarduino.com/upload/") + deviceToken);
                    http.addHeader("Content-Type", "image/jpeg");

                    int status = http.POST(jpeg, size);

                    if (status > 0) {
                        String response = http.getString();

                        return response.startsWith("http") ? "" : response;
                    }

                    return "Local request error";
                }
            };
        }
    }
}