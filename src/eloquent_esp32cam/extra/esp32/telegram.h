#ifndef ELOQUENT_EXTRA_ESP32_TELEGRAM
#define ELOQUENT_EXTRA_ESP32_TELEGRAM

#define _TELEGRAM_BOUNDARY "----WMAsrf456BGe4h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "./wifi/sta.h"
#include "../exception.h"

using namespace eloq;
using Eloquent::Error::Exception;

static const char _TELEGRAM_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEADCCAuigAwIBAgIBADANBgkqhkiG9w0BAQUFADBjMQswCQYDVQQGEwJVUzEh
MB8GA1UEChMYVGhlIEdvIERhZGR5IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBE
YWRkeSBDbGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTA0MDYyOTE3
MDYyMFoXDTM0MDYyOTE3MDYyMFowYzELMAkGA1UEBhMCVVMxITAfBgNVBAoTGFRo
ZSBHbyBEYWRkeSBHcm91cCwgSW5jLjExMC8GA1UECxMoR28gRGFkZHkgQ2xhc3Mg
MiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTCCASAwDQYJKoZIhvcNAQEBBQADggEN
ADCCAQgCggEBAN6d1+pXGEmhW+vXX0iG6r7d/+TvZxz0ZWizV3GgXne77ZtJ6XCA
PVYYYwhv2vLM0D9/AlQiVBDYsoHUwHU9S3/Hd8M+eKsaA7Ugay9qK7HFiH7Eux6w
wdhFJ2+qN1j3hybX2C32qRe3H3I2TqYXP2WYktsqbl2i/ojgC95/5Y0V4evLOtXi
EqITLdiOr18SPaAIBQi2XKVlOARFmR6jYGB0xUGlcmIbYsUfb18aQr4CUWWoriMY
avx4A6lNf4DD+qta/KFApMoZFv6yyO9ecw3ud72a9nmYvLEHZ6IVDd2gWMZEewo+
YihfukEHU1jPEX44dMX4/7VpkI+EdOqXG68CAQOjgcAwgb0wHQYDVR0OBBYEFNLE
sNKR1EwRcbNhyz2h/t2oatTjMIGNBgNVHSMEgYUwgYKAFNLEsNKR1EwRcbNhyz2h
/t2oatTjoWekZTBjMQswCQYDVQQGEwJVUzEhMB8GA1UEChMYVGhlIEdvIERhZGR5
IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBEYWRkeSBDbGFzcyAyIENlcnRpZmlj
YXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQAD
ggEBADJL87LKPpH8EsahB4yOd6AzBhRckB4Y9wimPQoZ+YeAEW5p5JYXMP80kWNy
OO7MHAGjHZQopDH2esRU1/blMVgDoszOYtuURXO1v0XJJLXVggKtI3lpjbi2Tc7P
TMozI+gciKqdi0FuFskg5YmezTvacPd+mSYgFFQlq25zheabIZ0KbIIOqPjCDPoQ
HmyW74cNxA9hi63ugyuV+I6ShHI56yDqg+2DzZduCLzrTia2cyvk0/ZM/iZx4mER
dEr/VxqHD3VILs9RaRegAhJhldXRQLIQTO7ErBBDpqWeCtWVYpoNz4iCxTIM5Cuf
ReYNnyicsbkqWletNw+vHX/bvZ8=
-----END CERTIFICATE-----
)EOF";


namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            /**
             * Telegram bot 
             */
            class Telegram {
            public:
                Exception exception;
                WiFiClientSecure client;

                /**
                 * Constructor
                 *
                 */
                Telegram() :
                        exception("Telegram"),
                        _token(""),
                        _chatId(""),
                        _requestTimeout(10000) {

                    #ifdef TELEGRAM_TOKEN
                    _token = TELEGRAM_TOKEN;
                    #endif

                }

                /**
                 * Set bot token
                 */
                void token(String token) {
                    _token = token;
                }

                /**
                 * Set chat id
                 */
                Telegram& to(String chatId) {
                    _chatId = chatId;

                    return *this;
                }

                /**
                 * Abort request after given time
                 */
                void timeoutAfter(size_t timeout) {
                    _requestTimeout = timeout;
                }

                /**
                 * Init bot
                 */
                Exception& begin(size_t timeout = 20000) {
                    if (!wifi.isConnected())
                        return exception.propagate(wifi);

                    if (client.connected())
                        return exception.clear();

                    client.setCACert(_TELEGRAM_CERT);
                    timeout += millis();

                    do {
                        client.connect("api.telegram.org", 443);
                        yield();
                        delay(1);
                    } while (millis() < timeout && wifi.isConnected() && !client.connected());

                    if (!client.connected())
                        return exception.set("Can't connect to Telegram API");

                    return exception.clear();
                }

                /**
                 * Send text message
                 */
                Exception& send(String text) {
                    if (!assertPrerequisites().isOk())
                        return exception;

                    client.print(F("POST https://api.telegram.org/bot"));
                    client.print(_token);
                    client.print(F("/sendMessage?chat_id="));
                    client.print(_chatId);
                    client.print(F("&text="));
                    client.print(text);
                    client.print(F(
                        " "
                        "HTTP/1.0\r\n"
                        "Host: api.telegram.org\r\n"
                        "Connection: close\r\n"
                        "Content-Type: application/json\r\n\r\n"
                    ));

                    return exception.set(getResponse());
                }

                /**
                 * Send photo
                 */
                template<typename Frame>
                Exception& send(Frame *fb) {
                    if (!assertPrerequisites().isOk())
                        return exception;

                    if (fb == NULL || !fb->len)
                        return exception.set("Cannot send empty frame");

                    String fileHeader = String("--" _TELEGRAM_BOUNDARY "\r\nContent-disposition: form-data; name=\"photo\"; filename=\"esp32cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n");
                    String endBoundary = String("\r\n--" _TELEGRAM_BOUNDARY "--\r\n");

                    client.print(F("POST https://api.telegram.org/bot"));
                    client.print(_token);
                    client.print(F("/sendPhoto?chat_id="));
                    client.print(_chatId);
                    client.print(F(
                        " "
                        "HTTP/1.0\r\n"
                        "Host: api.telegram.org\r\n"
                        "Connection: close\r\n"
                        "Content-Type: multipart/form-data; boundary=" _TELEGRAM_BOUNDARY "\r\n"
                        "Content-Length: "));
                    client.print(fb->len + fileHeader.length() + endBoundary.length());
                    client.print(F("\r\n\r\n"));
                    client.print(fileHeader);
                    client.write(fb->buf, fb->len);
                    client.println(endBoundary);

                    return exception.set(getResponse());
                }

            protected:
                size_t _requestTimeout;
                String _chatId;
                String _token;

                /**
                 * 
                 */
                Exception& assertPrerequisites() {
                    if (_token == "")
                        return exception.set("Bot token not set");

                    if (_chatId == "")
                        return exception.set("Chat ID not set");

                    if (!client.connected() && !begin().isOk())
                        return exception;

                    return exception.clear();
                }

                /**
                 * Test if response is ok
                 *
                 * @return
                 */
                String getResponse() {
                    size_t timeout = millis() + _requestTimeout;
                    bool success = false;
                    String response = "";

                    response.reserve(500);

                    while (client.connected() && millis() < timeout) {
                        if (!client.available())
                            delay(1);

                        response += (char) client.read();

                        // skip headers
                        if (response.length() < 200)
                            continue;

                        if (!success && response.indexOf("\"ok\":true") > -1) {
                            success = true;
                            break;
                        }

                        if (!success && response.indexOf("\"ok\":false") > -1)
                            break;
                    }

                    while (client.connected() && client.available())
                        response += (char) client.read();

                    client.flush();
                    client.stop();
                    log_d("Telegram response\n====%s\n====", response.c_str());

                    return success ? "" : response;
                }
            };
        }
    }
}


namespace eloq {
    static Eloquent::Extra::Esp32::Telegram telegram;
}

#endif
