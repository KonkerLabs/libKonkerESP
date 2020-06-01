#include "mqtt_protocol.h"

MQTTProtocol::MQTTProtocol() : ConnectionProtocol()
{
	mqttClient.setClient(espClient);
}

int MQTTProtocol::checkConnection()
{
	return mqttClient.connected();
}

int MQTTProtocol::connect()
{
	return mqttClient.connect(this->userid, this->userid, this->password);
}

int MQTTProtocol::disconnect()
{
	mqttClient.disconnect();
	return DISCONNECTED;
}
