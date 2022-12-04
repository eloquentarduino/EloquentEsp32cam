//
// Created by Simone on 02/12/22.
//

#ifndef ELOQUENTESP32CAM_TELEGRAM_H
#define ELOQUENTESP32CAM_TELEGRAM_H


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <AsyncTelegram2.h>
#include "../Cam.h"
#include "../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Http {
            /**
             * Send pictures to
             */
            class Telegram : public HasErrorMessage {
            public:
                AsyncTelegram2 bot;
                WiFiClientSecure client;

                /**
                 *
                 * @param bot
                 */
                Telegram(const char *token) :
                    bot(client),
                    _token(token),
                    _isInitialized(false) {

                }

                /**
                 * Init bot
                 *
                 * @return
                 */
                bool begin() {
                    if (_isInitialized)
                        return true;

                    bot.setTelegramToken(_token);
                    client.setCACert(telegram_cert);

                    if (!bot.begin())
                        return setErrorMessage("Cannot init bot");

                    return (_isInitialized = true);
                }

                /**
                 * Send text message
                 *
                 * @param chatId
                 * @param message
                 * @return
                 */
                bool sendMessage(const int64_t chatId, String message) {
                    if (!begin())
                        return false;

                    TBMessage msg;
                    msg.chatId = chatId;

                    if (bot.sendMessage(msg, message.c_str(), nullptr, true)) {
                        client.stop();

                        return setErrorMessage("");
                    }

                    client.stop();

                    return setErrorMessage("Send message error");
                }

                /**
                 * Send photo as attachment
                 *
                 * @param chatId
                 * @return
                 */
                bool sendPhoto(const int64_t chatId, Cam& cam) {
                    if (!begin())
                        return false;

                    if (!cam.captured())
                        return setErrorMessage("Cannot find frame");

                    bool response = bot.sendPhoto(chatId, cam.frame->buf, cam.frame->len);
                    client.stop();

                    return response;
                }

            protected:
                bool _isInitialized;
                const char *_token;
            };
        }
    }
}


#endif //ELOQUENTESP32CAM_TELEGRAM_H
