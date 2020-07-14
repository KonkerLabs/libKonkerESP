#include "mqtt_protocol.h"

MQTTProtocol::MQTTProtocol() : ConnectionProtocol()
{
	mqttClient.setClient(espClient);
}

void MQTTProtocol::protocolLoop()
{
	mqttClient.loop();
}

int MQTTProtocol::checkConnection()
{
	boolean res = mqttClient.connected();
	if (res)
	{
		return CONNECTED;
	}
	return NOT_CONNECTED;
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("] ");

	// TODO this
  // callSubChannelCallback(topic, payload, length);
}

int MQTTProtocol::connect()
{
	int connRes = 0;
	char user[PLAT_CRED_ARRAY_SIZE];
	char pwd[PLAT_CRED_ARRAY_SIZE];
	char host[PLAT_ADDR_ARRAY_SIZE];

	// string returned by c_str() does NOT works with mqttClient for some reason
	strncpy(user, this->getUser().c_str(), this->getUser().length());
	user[this->getUser().length()] = '\0';
	strncpy(pwd, this->getPassword().c_str(), this->getPassword().length());
	pwd[this->getPassword().length()] = '\0';
	strncpy(host, this->getHost().c_str(), this->getHost().length());
	host[this->getHost().length()] = '\0';

	if(mqttClient.connected())
	{
		Log.trace("[MQTT] Already connected to MQTT broker\n");
    return 1;
  }
	else
	{
		Log.trace("[MQTT] Going to connect to MQTT broker\n");
  }

	Log.trace("[MQTT] URI: %s Port: %d\n", host, this->getPort());

  mqttClient.setServer(host, this->getPort());
  mqttClient.setCallback(callback);

	Log.trace("[MQTT] USER: %s PASSWD: %s\n", user, pwd);

	for(int i=0; i<5; i++)
  {
		Log.trace("[MQTT] Connection attempts left %d\n", 5 - i);
		connRes = mqttClient.connect(user, user, pwd);
    // connRes = mqttClient.connect(this->getUser().c_str(),
		// 														 this->getUser().c_str(),
		// 														 this->getPassword().c_str());
    if(connRes) break;
    delay(1500);
  }

	Log.trace("[MQTT] Connection response = %d\n", connRes);

	//Check the returning code
	if (connRes == 1)
	{
    Log.trace("[MQTT] Connected to MQTT broker\n\n");
    return 1;
  }
	else
	{
    Log.trace("[MQTT] Failed to connect to MQTT broker.\n");
    Log.trace("[MQTT] State = %d\n\n", mqttClient.state());
    // appendToFile(healthFile,(char*)"1", _mqttFailureAdress);
    delay(3000);
#ifndef ESP32
    ESP.reset();
#else
    ESP.restart();
#endif
    return 0;
  }
}

int MQTTProtocol::disconnect()
{
	mqttClient.disconnect();
	return DISCONNECTED;
}

int MQTTProtocol::send(const char * channel, String payload)
{
	int pubCode = -1;
	char topic[32];
	String topicStr = this->prefix + "/" + this->getUser() + "/pub/" + channel;

  strcpy(topic, topicStr.c_str());
	Log.notice("[MQTT] Publishing to: %s | Message: %s\n", topic, payload.c_str());

	pubCode = mqttClient.publish(topic, payload.c_str(), payload.length());

	if (pubCode != 1)
	{
		Log.notice("[MQTT] Failed. Code = %d\n", pubCode);
    // failedComm=1;
    // appendToFile(healthFile,(char*)"1", _mqttFailureAdress);
    delay(3000);
#ifndef ESP32
    ESP.reset();
#else
    ESP.restart();
#endif
    // return 0;
  }
	else
	{
		Log.notice("[MQTT] Sucess. Code = %d\n", pubCode);
    // return 1;
  }

	return pubCode;
}

void MQTTProtocol::setPrefix(String p)
{
	this->prefix = p;
}
