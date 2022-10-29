// netpie.h
// dew.ninja  October 2022
// netpie class definition

#ifndef NETPIE_H
#define NETPIE_H

#include <Arduino.h>
#include <PubSubClient.h>

extern char msg[];

class NETPIE   {
    private:
        char* _mqtt_client;
        char* _mqtt_username;
        char* _mqtt_password;
        char* _mqtt_server;
        int _mqtt_port;
        PubSubClient _client;
    public:
        NETPIE(char* mqtt_client, char* mqtt_username, char* mqtt_password, PubSubClient &client);
        void setServer();
        void setCallback(MQTT_CALLBACK_SIGNATURE);
        void reconnect();
        void stayconnected();
        boolean loop();
        boolean connected();
        boolean publish(const char* topic, const char* payload);
         
};


#endif