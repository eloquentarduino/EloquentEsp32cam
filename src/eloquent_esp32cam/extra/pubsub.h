#ifndef PubSubClient_h
#else
#ifndef ELOQUENT_EXTRA_PUBSUB_H
#define ELOQUENT_EXTRA_PUBSUB_H 1

#include <WiFi.h>
#include <PubSubClient.h>
#include "./exception.h"

using Eloquent::Error::Exception;


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
            void clientName(String clientName) {
                clientId = clientName;
                
                // replace vars
                clientId.replace("{mac}", WiFi.macAddress());
            }
                
            /**
             * @brief Set MQTT server
             * @param server
             */
            void server(const char* server, uint16_t port = 0) {
                mqtt.setServer(server, port ? port : _port);
            }
            
            /**
             * @brief Set username/password for broker
             * @param user
             * @param pass
             */
            void auth(String user, String pass) {
                _user = user;
                _pass = pass;
            }
            
            /**
             * @brief Connect to MQTT client
             */
            Exception& connect(size_t timeout = 10000) {
                if (mqtt.connected())
                    return exception.clear();
                    
                if (!_server)
                    return exception.set("You must set an MQTT broker with mqtt.server(serverName)");
                    
                if (WiFi.status() != WL_CONNECTED)
                    return exception.set("You must be connected to WiFi");
                    
                timeout += millis();
                
                while (millis() < timeout) {
                    const char *user = _user == "" ? NULL : _user.c_str();
                    const char *pass = _pass == "" ? NULL : _pass.c_str();
                    
                    if (mqtt.connect(clientId.c_str(), user, pass))
                        return exception.clear();
                }
                    
                return exception.set("Cannot connect to MQTT server");
            }
        
            /**
             * @brief Send MQTT message
             * @param topic
             */
            Exception& publish(String topic) {
                //if (!_subject->shouldPub())
                //    return exception;
                    
                if (!connect().isOk())
                    return exception;
                   
                if (!mqtt.publish(topic.c_str(), _subject->toJSON().c_str()))
                    return exception.set("Cannot send MQTT message");
                    
                return exception.clear();
            }
            
        protected:
            T *_subject;
            uint16_t _port;
            String _server;
            String _user;
            String _pass;
        };
    }
}


#endif
#endif