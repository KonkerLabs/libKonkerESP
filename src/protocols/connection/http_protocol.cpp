#include "http_protocol.h"

HTTPProtocol::HTTPProtocol() : http_client()
{
  // Log.trace("[HTTP] Creating HTTP object\n");
  this->connectionOriented = true;
  // maybe set this to false to save battery (no need to keep alive)
  this->http_client.setReuse(true);
}

HTTPProtocol::~HTTPProtocol()
{
  if (this->http_client.connected())
  {
    this->http_client.end();
  }
}

void HTTPProtocol::setupClient(void * http, String uri)
{
  char user[PLAT_ADDR_ARRAY_SIZE];
  char passwd[PLAT_ADDR_ARRAY_SIZE];

  strncpy(user, this->getUser().c_str(), this->getUser().length());
	user[this->getUser().length()] = '\0';
	strncpy(passwd, this->getPassword().c_str(), this->getPassword().length());
	passwd[this->getPassword().length()] = '\0';

  this->http_client.setAuthorization(user, passwd);
  this->http_client.begin(this->wifi_client, "http://"+this->getHost(), this->getPort(), this->registry + uri);
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
    Log.trace("[HTTP] Cannot connect to platform\n");
    this->numConnFail++;
    return NOT_CONNECTED;
  }

  Log.trace("[HTTP] Connected to platform\n");
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
  // this->http_client.end();

  if (httpCode >= 0 && httpCode < 300)
  {
    Log.notice("[HTTP] Success. Code = %d\n", httpCode);
    return 1;
  }
  else
  {
    Log.notice("[HTTP] Failed. Code = %d\n", httpCode);
    this->numConnFail++;
    if(httpCode < 0)
      return 0;
    return httpCode;
  }
}

int HTTPProtocol::receive(String *payload)
{
  return 0;
}

int HTTPProtocol::request(String * retPayload, String endpoint)
{
  int ret = 0;
  char user[PLAT_ADDR_ARRAY_SIZE];
  char passwd[PLAT_ADDR_ARRAY_SIZE];

  strncpy(user, this->getUser().c_str(), this->getUser().length());
	user[this->getUser().length()] = '\0';
	strncpy(passwd, this->getPassword().c_str(), this->getPassword().length());
	passwd[this->getPassword().length()] = '\0';

  char buffer[100];
  String bufferStr = "http://" + this->getHost() + "/" + this->registry + endpoint;
  strcpy(buffer, bufferStr.c_str());

  this->http_client.addHeader("Content-Type", "application/json");
  this->http_client.setTimeout(2000);
  this->http_client.setAuthorization(user, passwd);
  this->http_client.begin(this->wifi_client, buffer);  //Specify request destination
  int httpCode = this->http_client.GET();

  if (httpCode >= 0 && httpCode < 300)
  {
    Log.trace("[HTTP] Request sucess. Code = %d\n", httpCode);

    *retPayload = this->http_client.getString();
    Log.trace("[HTTP] retPayload = %s\n", retPayload->c_str());
    // int playloadSize=this->http_client.getSize();

    // getCurrentTime(ts, ms);
    return 1;
  }
  else
  {
    Log.trace("[HTTP] Request failed. Code = %d\n", httpCode);
    return 0;
  }

  return ret;
}
