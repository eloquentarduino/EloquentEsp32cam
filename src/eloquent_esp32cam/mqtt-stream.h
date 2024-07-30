#pragma once

#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 30000
#endif

#include <WiFi.h>
#include <MQTT.h>
#include "./extra/exception.h"

using Eloquent::Error::Exception;

typedef struct {
    uint8_t *buf;
    uint32_t len;
} mqtt_frame_t;

namespace Eloquent {
    namespace Esp32cam {
        /**
         * Stream camera frames over MQTT
         */
        class MqttStream {
        public:
            Exception exception;

            /**
             *
             */
            MqttStream() :
                exception("MqttStream"),
                _throttle(100) {

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
             * @return
             */
            Exception& begin(String clientID) {
                _clientID = clientID;
                _mqttClient.begin("mqtt.eloquentarduino.com", 1883, _wifiClient);

                if (WiFi.status() != WL_CONNECTED)
                    return exception.set("WiFi not connected");

                if (!_mqttClient.connect(_clientID.c_str(), _clientID.c_str(), "password")) {
                    ESP_LOGE("MqttStream", "Can't connect to MQTT broker. Are you sure your CLIENT_ID is valid?");
                    return exception.set("Can't connect to MQTT broker");
                }

                _queue = xQueueCreate(1, sizeof(mqtt_frame_t));

                xTaskCreate([](void *args) {
                    MqttStream *self = (MqttStream*) args;
                    mqtt_frame_t frame;
                    String topic = String("/") + self->_clientID + "/stream";

                    while (true) {
                        // await for new frame
                        if (xQueueReceive(self->_queue, &frame, 10) != pdPASS)
                            continue;

                        if (WiFi.status() != WL_CONNECTED) {
                            ESP_LOGE("MqttStream", "WiFi not connected");
                            delay(500);
                            continue;
                        }

                        if (!self->_mqttClient.connected()) {
                            ESP_LOGE("MqttStream", "Can't connect to MQTT broker. Are you sure your CLIENT_ID is valid?");
                            delay(10);
                            continue;
                        }

                        // publish
                        if (frame.len > 0) {
                            if (frame.len > MQTT_MAX_PACKET_SIZE) {
                                ESP_LOGE("MqttStream", "Frame too large (%d bytes, max %d allowed)", frame.len, MQTT_MAX_PACKET_SIZE);
                                continue;
                            }

                            if (!self->_mqttClient.publish(topic.c_str(), (const char*) frame.buf, frame.len))
                                ESP_LOGE("MqttStream", "Can't publish frame");
                        }

//                        self->_mqttClient.loop();
                    }
                }, "MqttStream", 50000, this, 0, NULL);

                return exception.clear();
            }

            /**
             *
             * @tparam Camera
             * @param camera
             * @return
             */
            template<typename Camera>
            Exception& queue(Camera& camera) {
                if (_queue == NULL)
                    return exception.set("Queue is not initialized. Did you call begin()?");

                const size_t now = millis();

                // throttle
                if (now > _queuedAt && now - _queuedAt < _throttle)
                    return exception.clear();

                // discard old frame, if not consumed yet
                if (uxQueueSpacesAvailable(_queue) > 0)
                    xQueueReset(_queue);

                if (!camera.hasFrame())
                    return exception.set("Cannot queue empty frame");

                mqtt_frame_t frame = {
                        .buf = camera.frame->buf,
                        .len = camera.frame->len
                };

                if (xQueueSendToFront(_queue, (void *) &frame, 0) != pdTRUE)
                    return exception.set("Cannot queue frame");

                _queuedAt = millis();
                return exception.clear();
            }

        protected:
            size_t _queuedAt;
            size_t _throttle;
            String _clientID;
            String _location;
            WiFiClient _wifiClient;
            MQTTClient _mqttClient;
            QueueHandle_t _queue;
        };
    }
}

namespace eloq {
    static Eloquent::Esp32cam::MqttStream mqttStream;
}