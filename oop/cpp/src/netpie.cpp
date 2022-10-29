// netpie.cpp
// dew.ninja  October 2022
// implement netpie class using object-oriented approach

#include "netpie.h"

NETPIE::NETPIE(char* mqtt_client, char* mqtt_username, char* mqtt_password, PubSubClient &client)  {
    _mqtt_client = mqtt_client;
    _mqtt_username = mqtt_username;
    _mqtt_password = mqtt_password; 
    _mqtt_server = "broker.netpie.io";
    _mqtt_port =  1883;
    _client = client;     

}

void NETPIE::setServer()   {
  _client.setServer(_mqtt_server, _mqtt_port);
}

void NETPIE::setCallback(MQTT_CALLBACK_SIGNATURE)  {
  _client.setCallback(callback); 
}

void NETPIE::reconnect() {
  while (!_client.connected()) {
    Serial.print("Attempting MQTT connection…");
    if (_client.connect(_mqtt_client, _mqtt_username, _mqtt_password)) {
      Serial.println("connected");
      // subscribe to command topics
      _client.subscribe("@msg/#");      
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(_client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

boolean NETPIE::connected()   {
  return _client.connected();
}

boolean NETPIE::loop()   {
  return _client.loop();
}

boolean NETPIE::publish(const char* topic, const char* payload)  {
  return _client.publish(topic, payload);

}
 
void NETPIE::stayconnected()   {
  if (!_client.connected()) {
    reconnect();
  }
  _client.loop();

}