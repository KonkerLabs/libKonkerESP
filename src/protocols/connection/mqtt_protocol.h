#ifndef __MQTT_PROTOCOL_H__
#define __MQTT_PROTOCOL_H__

#include <PubSubClient.h>
#include "connection_protocol.h"

class MQTTProtocol: public ConnectionProtocol
{
private:
	WiFiClient espClient;
	PubSubClient mqttClient;

	String prefix = "data";
public:
	MQTTProtocol();

	int connect() override;
	int disconnect() override;
	int checkConnection() override;

	int send(const char * channel, String payload);

	bool protocolLoop() override;

	void setPrefix(String p);
};

#endif /* __MQTT_PROTOCOL_H__ */
