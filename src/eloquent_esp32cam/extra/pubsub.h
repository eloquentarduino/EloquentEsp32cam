#ifndef PubSubClient_h
#else
#ifndef ELOQUENT_EXTRA_PUBSUB_H
#define ELOQUENT_EXTRA_PUBSUB_H 1

#include <WiFi.h>
#include <PubSubClient.h>
#include "./exception.h"

using Eloquent::Extra::Exception;


namespace Eloquent {
    namespace Extra {
        /**
         * @class PubSub
         * @author Simone
         * @date 13/12/2023
         * @file pubsub.h
         * @brief Send data over MQTT
         */
         template<typename T>
        class PubSub {
        public:
            String clientId;
            WiFiClient client;
            PubSubClient mqtt;
            Exception exception;
        
            /**
             * @brief Constructor
             * @param subject
             */
            PubSub(T *subject) :
                _subject(subject),
                _server(""),
                _port(1883),
                mqtt(client),
                exception("PubSub") {
                    // generate client id based on MAC address
                    clientId = String("pubsub-") + WiFi.macAddress();
                }
                
            /**
             * @brief Set client id
             * @param clientName
             */
            void name(String clientName) {
                clientId = clientName;
                
                // replace vars
                clientId.replace("{mac}", WiFi.macAddress());
            }
                
            /**
             * @brief Set MQTT server
             * @param server
             */
            void server(String server, uint16_t port = 0) {
                _server = server;
                _port = port ? port : _port;
            }
            
            /**
             * @brief Connect to MQTT client
             */
            Exception& connect() {
                if (mqtt.connected())
                    return exception.clear();
                    
                if (!_server)
                    return exception.set("You must set an MQTT broker with mqtt.server(serverName)");
                    
                if (WiFi.status() != WL_CONNECTED)
                    return exception.set("You must be connected to WiFi");
                    
                if (!mqtt.connect(clientId.c_str()))
                    return exception.set("Cannot connect to MQTT server");
                    
                return exception.clear();
            }
        
            /**
             * @brief Send MQTT message
             * @param topic
             */
            Exception& publish(String topic) {
                if (!_subject->shouldPub())
                    return exception;
                    
                if (!connect())
                    return exception;
                    
                mqtt.publish(topic.c_str(), _subject->toJSON().c_str());
                    
                return exception.clear();
            }
            
        protected:
            T *_subject;
            uint16_t _port;
            String _server;
        };
    }
}


#endif
#endif