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
  bool conn = false;

  if (!this->isCredentialSet())
	{
    Log.trace("[HTTP] Platform credentials are missing!\n");
    Log.trace("[HTTP] Trying to restore from EEPROM\n");
		if(!this->restorePlatformCredentials())
		{
			Log.warning("[HTTP] Credentials not found! Aborting\n");
      this->numConnFail++;
			return NOT_CONNECTED;
		}
	}

  conn = this->http_client.begin(wifi_client, "http://"+this->getHost()+":"+this->getPort());

  if(!conn)
  {
    this->numConnFail++;
    return NOT_CONNECTED;
  }

  return CONNECTED;
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

int HTTPProtocol::send(const char *channel, String payload)
{
  char user[PLAT_ADDR_ARRAY_SIZE];
  char passwd[PLAT_ADDR_ARRAY_SIZE];

  strncpy(user, this->getUser().c_str(), this->getUser().length());
	user[this->getUser().length()] = '\0';
	strncpy(passwd, this->getPassword().c_str(), this->getPassword().length());
	passwd[this->getPassword().length()] = '\0';

  this->http_client.addHeader("Content-Type", "application/json");
  this->http_client.addHeader("Accept", "application/json");
  Log.trace("[HTTP] Authorization: %s<>%s\n", user, passwd);
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
    return CONNECTED;
  }
  else
  {
    Log.notice("[HTTP] Failed. Code = %d\n", httpCode);
    this->numConnFail++;
    if(httpCode < 0)
      return DISCONNECTED;
    return NOT_CONNECTED;
  }
}

int HTTPProtocol::receive(String *payload)
{
  return 0;
}
