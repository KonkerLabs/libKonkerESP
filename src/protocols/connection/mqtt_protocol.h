#ifndef __MQTT_PROTOCOL_H__
#define __MQTT_PROTOCOL_H__

#include <PubSubClient.h>
#include "connection_protocol.h"

class MQTTProtocol: public ConnectionProtocol
{
private:
	WiFiClient espClient;
	PubSubClient mqttClient;
public:
	MQTTProtocol();

	int connect() override;
  int disconnect() override;
  int checkConnection() override;
};

#endif /* __MQTT_PROTOCOL_H__ */
