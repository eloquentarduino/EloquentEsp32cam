//
// Created by Simone on 02/12/22.
//

#ifndef ELOQUENTESP32CAM_TELEGRAM_H
#define ELOQUENTESP32CAM_TELEGRAM_H

#ifndef TELEGRAM_HOST
#define TELEGRAM_HOST "api.telegram.org"
#endif

#ifndef TELEGRAM_PORT
#define TELEGRAM_PORT 443
#endif


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "../Cam.h"
#include "../../traits/HasErrorMessage.h"


static const char TELEGRAM_CERT[] PROGMEM = R"EOF(
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
    namespace Esp32cam {
        namespace Http {
            /**
             * Send pictures to Telegram
             * @bug can only send 1 picture (cannot figure out why)
             */
            class TelegramChat : public HasErrorMessage {
            public:
                WiFiClientSecure client;

                /**
                 *
                 * @param bot
                 */
                TelegramChat(const char *token, String chatId) :
                    _chatId(chatId),
                    _token(token),
                    _requestTimeout(10000) {

                }

                /**
                 * Abort request after given time
                 *
                 * @param timeout
                 */
                void setRequestTimeout(size_t timeout) {
                    _requestTimeout = timeout;
                }

                /**
                 * Init bot
                 *
                 * @return
                 */
                bool begin(size_t connectionTimeout = 20000) {
                    if (client.connected())
                        return setErrorMessage("");

                    client.setCACert(TELEGRAM_CERT);

                    size_t timeout = millis() + connectionTimeout;

                    do {
                        client.connect(TELEGRAM_HOST, TELEGRAM_PORT);
                    } while (millis() < timeout && !client.connected());

                    if (!client.connected())
                        return setErrorMessage("Client not connected");

                    return setErrorMessage("");
                }

                /**
                 *
                 * @param text
                 * @return
                 */
                bool sendText(String text) {
                    client.print(F("POST https://" TELEGRAM_HOST "/bot"));
                    client.print(_token);
                    client.print(F("/sendMessage?chat_id="));
                    client.print(_chatId);
                    client.print(F("&text="));
                    client.print(text);
                    client.print(F(
                            " HTTP/1.0"
                            "\nHost: api.telegram.org"
                            "\nConnection: keep-alive"
                            "\nContent-Type: application/json\n\n"));

                    return requestIsSuccessful();
                }

                /**
                 * Send photo as attachment
                 *
                 * @param chatId
                 * @return
                 */
                bool sendPhoto(Cam& cam) {
                    #define BOUNDARY "----WMAsrf456BGe4h"

                    if (!begin())
                        return false;

                    if (!cam.captured())
                        return setErrorMessage("Cannot find frame");

                    String fileHeader = String("--" BOUNDARY "\r\nContent-disposition: form-data; name=\"photo\"; filename=\"image.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n");
                    String endBoundary = String("\r\n--" BOUNDARY "--\r\n");

                    client.print(F("POST https://" TELEGRAM_HOST "/bot"));
                    client.print(_token);
                    client.print(F("/sendPhoto?chat_id="));
                    client.print(_chatId);
                    client.print(F(
                            " HTTP/1.0"
                            "\r\nHost: api.telegram.org"
                            "\r\nConnection: keep-alive"
                            "\r\nContent-Type: multipart/form-data; boundary=" BOUNDARY
                            "\r\nContent-Length: "));
                    client.print(cam.frame->len + fileHeader.length() + endBoundary.length());
                    client.print(F("\r\n\r\n"));
                    client.print(fileHeader);
                    client.write(cam.frame->buf, cam.frame->len);
                    client.println(endBoundary);

                    return requestIsSuccessful();
                }

            protected:
                size_t _requestTimeout;
                const String _chatId;
                const char *_token;


                /**
                 * Test if response is ok
                 *
                 * @return
                 */
                bool requestIsSuccessful() {
                    size_t timeout = millis() + _requestTimeout;
                    bool success = false;
                    uint16_t len = 0;
                    String response;

                    while (client.connected() && millis() < timeout) {
                        if (!client.available())
                            delay(1);

                        yield();
                        len += 1;

                        // skip headers
                        if (len < 200) {
                            client.read();
                            continue;
                        }

                        response += (char) client.read();

                        if (response.indexOf("\"ok\":true") > -1) {
                            success = true;
                            break;
                        }

                        if (response.indexOf("\"ok\":false") > -1)
                            break;
                    }

                    while (client.connected() && client.available())
                        client.read();

                    client.flush();

                    return setErrorMessage(success ? "" : "Bad response");
                }
            };
        }
    }
}


#endif //ELOQUENTESP32CAM_TELEGRAM_H
