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
             * Send pictures to Telegram
             * @bug can only send 1 picture (cannot figure out why)
             */
            class TelegramChat : public HasErrorMessage {
            public:
                AsyncTelegram2 bot;
                WiFiClientSecure client;

                /**
                 *
                 * @param bot
                 */
                TelegramChat(const char *token, const int64_t chatId) :
                    _chatId(chatId),
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

                    client.setCACert(telegram_cert);
                    bot.setTelegramToken(_token);

                    if (!bot.begin())
                        return setErrorMessage("Cannot init bot");

                    if (!client.connected())
                        return setErrorMessage("Client not connected");

                    return (_isInitialized = true);
                }

                /**
                 * Send text message
                 *
                 * @param message
                 * @return
                 */
                bool sendMessage(String message) {
                    if (!begin())
                        return false;

                    if (bot.sendTo(_chatId, message)) {
                        return setErrorMessage("");
                    }

                    return setErrorMessage("Send message error");
                }

                /**
                 * Send photo as attachment
                 *
                 * @param chatId
                 * @return
                 */
                bool sendPhoto(Cam& cam) {
                    if (!begin())
                        return false;

                    if (!cam.captured())
                        return setErrorMessage("Cannot find frame");

                    bool response = bot.sendPhoto(_chatId, cam.frame->buf, cam.frame->len);

                    return setErrorMessage(response ? "" : "Cannot send to Telegram");
                }

            protected:
                bool _isInitialized;
                const int64_t _chatId;
                const char *_token;
            };
        }
    }
}


#endif //ELOQUENTESP32CAM_TELEGRAM_H
