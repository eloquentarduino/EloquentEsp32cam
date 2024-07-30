#pragma once

#include <WiFi.h>
#include "./extra/exception.h"

namespace Eloquent {
    class RemoteStream {
    public:
        Exception exception;

        /**
         *
         */
        RemoteStream() :
            exception("RemoteStream"),
            _throttle(100),
            _sentAt(0) {

        }

        /**
         * Don't send frames too often
         * @param ms
         */
        void throttle(size_t ms) {
            _throttle = ms;
        }

        /**
         *
         * @param clientID
         * @return
         */
        Exception& begin(String clientID) {
            _clientID = clientID;

            if (WiFi.status() != WL_CONNECTED)
                return exception.set("WiFi not connected");

            if (!_wifiClient.connect(getServerName().c_str(), 10300))
                return exception.set("Can't connect to streaming server");

            return exception.clear();
        }

        /**
         *
         * @param frame
         * @return
         */
        Exception& send(camera_fb_t *frame) {
            const size_t now = millis();

            if (now > _sentAt && now - _sentAt < _throttle)
                return exception.clear();

            if (!_wifiClient.connected())
                _wifiClient.connect(getServerName().c_str(), 10300);

            if (!_wifiClient.connected())
                return exception.set("Can't connect to streaming server");

            _wifiClient.print("##SOF##");
            _wifiClient.print(_clientID);
            _wifiClient.print("##DATA##");
            _wifiClient.write(frame->buf, frame->len);
            _wifiClient.print("##EOF##");
            _wifiClient.flush();

            return exception.clear();
        }

        /**
         *
         * @return
         */
        String address() {
            return String("http://") + getServerName() + "/esp32-cam-stream?client_id=" + _clientID;
        }

    protected:
        size_t _throttle;
        size_t _sentAt;
        String _clientID;
        WiFiClient _wifiClient;

        /**
         *
         * @return
         */
        String getServerName() {
            return "mqtt.eloquentarduino.com";
        }
    };
}

namespace eloq {
    static Eloquent::RemoteStream remoteStream;
}