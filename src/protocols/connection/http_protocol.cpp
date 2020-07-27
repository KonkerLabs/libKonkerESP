#include "http_protocol.h"

HTTPProtocol::HTTPProtocol() : http_client()
{
  // Log.trace("[HTTP] Creating HTTP object\n");
  this->connectionOriented = true;
}

HTTPProtocol::~HTTPProtocol()
{
  if (this->http_client.connected())
  {
    this->http_client.end();
  }
}

void HTTPProtocol::getClient(HTTPClient* http)
{
  http = &this->http_client;
}

int HTTPProtocol::connect()
{
  bool conn = 0;

  if (!this->isCredentialSet())
	{
    Log.trace("[HTTP] Platform credentials are missing!\n");
    Log.trace("[HTTP] Trying to restore from EEPROM\n");
		if(!this->restorePlatformCredentials())
		{
			Log.warning("[HTTP] Credentials not found! Aborting\n");
      this->numConnFail++;
			return 0;
		}
	}

  conn = this->http_client.begin(wifi_client, "http://"+this->getHost()+":"+this->getPort());

  if(!conn) this->numConnFail++;

  return conn;
}

int HTTPProtocol::disconnect()
{
  if (this->http_client.connected())
  {
    this->http_client.end();
  }
  return DISCONNECTED;
}
int HTTPProtocol::checkConnection()
{
  return this->http_client.connected();
}

void HTTPProtocol::buildHTTPSUBTopic(char const channel[], char *topic)
{
  char bffPort[6];
// //  itoa (_rootPort,bffPort,10);
//   itoa(port, bffPort, 10);
//   strcpy (topic,"http://");
//   strcat (topic,host.c_str());
//   strcat (topic,":");
//   strcat (topic,bffPort);
//   strcat (topic,"/");
//   strcat (topic,SUB_PREFIX);
//   strcat(topic,"/");
//   strcat (topic,this->userid);
//   strcat(topic,"/");
//   strcat (topic,channel);
}

int HTTPProtocol::send(const char *channel, String payload)
{
  // TODO move this throtle to helth later
  //throtle this call
  // if ((millis()-_last_time_http_request) < _millis_delay_per_http_request)
  // {
  //   delay((millis()-_last_time_http_request));
  // }
  // _last_time_http_request = millis();
  const char * user = this->getUser().c_str();
  const char * passwd = this->getPassword().c_str();

  this->http_client.addHeader("Content-Type", "application/json");
  this->http_client.addHeader("Accept", "application/json");
  this->http_client.setAuthorization(user, passwd);

  char buffer[100];

  String bufferStr = "http://" + this->getHost() + ":"
                      + String(this->getPort()) + "/" + this->registry
                      + "pub/" + this->getUser() + "/" + channel;
  strcpy(buffer, bufferStr.c_str());

  Log.trace("[HTTP] Publishing to %s\n", buffer);
  Log.trace("[HTTP] Body %s\n", payload.c_str());

  this->http_client.setTimeout(10000);
  this->http_client.begin(this->wifi_client, bufferStr);

  Log.notice("[HTTP] (B) POST TO DATA\n");

  int httpCode = this->http_client.POST(payload);
  Log.notice("[HTTP] (E) POST TO DATA\n");

  if (httpCode >= 0 && httpCode < 300)
  {
    Log.notice("[HTTP] Success. Code = %d\n", httpCode);
    return 1;
  }
  else
  {
    Log.notice("[HTTP] Failed. Code = %d\n", httpCode);
    failedComm = 1;
    return 0;
  }
}

int HTTPProtocol::receive(String *payload)
{
  return 0;
}
