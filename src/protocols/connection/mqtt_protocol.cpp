#include "mqtt_protocol.h"

MQTTProtocol::MQTTProtocol() : ConnectionProtocol()
{
	mqttClient.setClient(espClient);
	this->connectionOriented = true;
}

bool MQTTProtocol::protocolLoop()
{
	mqttClient.loop();
	return BaseProtocol::protocolLoop();
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

	// TODO this callSubChannelCallback
  // callSubChannelCallback(topic, payload, length);
}

int MQTTProtocol::connect()
{
	int connRes = 0;
	char user[PLAT_CRED_ARRAY_SIZE];
	char pwd[PLAT_CRED_ARRAY_SIZE];
	char host[PLAT_ADDR_ARRAY_SIZE];

	if (!this->isCredentialSet())
	{
		Log.trace("[MQTT] Platform credentials are missing!\n");
		Log.trace("[MQTT] Trying to restore from EEPROM\n");
		if(!this->restorePlatformCredentials())
		{
			Log.warning("[MQTT] Credentials not found! Aborting\n");
			this->numConnFail++;
			return 0;
		}
	}

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
    return CONNECTED;
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
    return CONNECTED;
  }
	else
	{
    Log.trace("[MQTT] Failed to connect to MQTT broker.\n");
    Log.trace("[MQTT] State = %d\n\n", mqttClient.state());
    this->numConnFail++;

    return NOT_CONNECTED;
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

	if (pubCode)
	{
		Log.notice("[MQTT] Sucess. Code = %d\n", pubCode);
  }
	else
	{
		Log.notice("[MQTT] Failed. Code = %d\n", pubCode);
		this->numConnFail++;
		if(mqttClient.state() < 0)
			return DISCONNECTED;
  }

	return pubCode;
}

void MQTTProtocol::setPrefix(String p)
{
	this->prefix = p;
}
