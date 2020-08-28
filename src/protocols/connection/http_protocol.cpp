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

int HTTPProtocol::connect()
{
  bool conn = false;

  // if (!this->isCredentialSet())
	// {
  //   Log.trace("[HTTP] Platform credentials are missing!\n");
  //   Log.trace("[HTTP] Trying to restore from EEPROM\n");
	// 	if(!this->restorePlatformCredentials())
	// 	{
	// 		Log.warning("[HTTP] Credentials not found! Aborting\n");
  //     this->numConnFail++;
	// 		return NO_CREDENTIALS;
	// 	}
	// }

  conn = this->http_client.begin(wifi_client, "http://"+this->getHost()+":"+this->getPort());

  if(!conn)
  {
    Log.trace("[HTTP] Cannot connect to platform\n");
    this->numConnFail++;
    return NOT_CONNECTED;
  }

  Log.trace("[HTTP] Connected to platform\n\n");
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

  String bufferStr = "http://" + this->getHost() + ":"
                      + String(this->getPort()) + "/" + this->registry
                      + "pub/" + this->getUser() + "/" + channel;

  this->http_client.begin(this->wifi_client, bufferStr);
  Log.trace("[HTTP] Publishing to %s\n", bufferStr.c_str());
  Log.trace("[HTTP] Body %s\n", payload.c_str());

  Log.trace("[HTTP] Authorization: %s<>%s\n", user, passwd);
  this->http_client.setAuthorization(user, passwd);
  this->http_client.setTimeout(10000);
  this->http_client.addHeader("Content-Type", "application/json");
  this->http_client.addHeader("Accept", "application/json");

  Log.notice("[HTTP] (B) POST TO DATA\n");

  int httpCode = this->http_client.POST(payload);
  Log.notice("[HTTP] (E) POST TO DATA\n");
  this->http_client.end();

  if (httpCode >= 0 && httpCode < 300)
  {
    Log.notice("[HTTP] Success. Code(%d)\n", httpCode);
    return 1;
  }
  Log.notice("[HTTP] Failed. Code(%d): %s\n", httpCode, this->http_client.errorToString(httpCode).c_str());
  this->numConnFail++;
  if(httpCode < 0)
    return 0;
  return httpCode;
}

int HTTPProtocol::receive(String *payload)
{
  return 0;
}

int HTTPProtocol::request(String * retPayload, String endpoint)
{
  char user[PLAT_ADDR_ARRAY_SIZE];
  char passwd[PLAT_ADDR_ARRAY_SIZE];

  strncpy(user, this->getUser().c_str(), this->getUser().length());
	user[this->getUser().length()] = '\0';
	strncpy(passwd, this->getPassword().c_str(), this->getPassword().length());
	passwd[this->getPassword().length()] = '\0';

  String bufferStr = "http://" + this->getHost() + ":" + String(this->getPort())
                      + "/" + this->registry + endpoint;

  this->http_client.begin(this->wifi_client, bufferStr.c_str());
  Log.trace("[HTTP] Requesting from %s\n", bufferStr.c_str());

  this->http_client.setTimeout(2000);
  this->http_client.setAuthorization(user, passwd);
  this->http_client.addHeader("Content-Type", "application/json");
  this->http_client.addHeader("Accept", "application/json");

  int httpCode = this->http_client.GET();

  if (httpCode >= 0 && httpCode < 300)
  {
    Log.trace("[HTTP] Request sucess. Code(%d)\n", httpCode);

    *retPayload = this->http_client.getString();
    Log.trace("[HTTP] retPayload[%d bytes] = %s\n", this->http_client.getSize(), retPayload->c_str());
    this->http_client.end();

    return 1;
  }
  Log.trace("[HTTP] Request failed. Code(%d): %s\n", httpCode, this->http_client.errorToString(httpCode).c_str());
  this->http_client.end();
  if(httpCode < 0)
    return 0;
  return httpCode;
}

bool HTTPProtocol::getPlatformCredentials(String * response, String id)
{
  Log.trace("[HTTP] Getting platform credentials from gateway\n");

  String server = "192.168.0.105";
  // char user[PLAT_ADDR_ARRAY_SIZE];
  // char passwd[PLAT_ADDR_ARRAY_SIZE];
  //
  // strncpy(user, this->getUser().c_str(), this->getUser().length());
	// user[this->getUser().length()] = '\0';
	// strncpy(passwd, this->getPassword().c_str(), this->getPassword().length());
	// passwd[this->getPassword().length()] = '\0';

  String bufferStr = "http://" + server + ":" + "8089"
                      + "/credentials?id=" + id;

  this->http_client.begin(this->wifi_client, bufferStr.c_str());
  Log.trace("[HTTP] Requesting from %s\n", bufferStr.c_str());

  this->http_client.setTimeout(5000);
  this->http_client.addHeader("Content-Type", "application/json");
  this->http_client.addHeader("Accept", "application/json");
  int httpCode = this->http_client.GET();

  if (httpCode >= 0 && httpCode < 300)
  {
    Log.trace("[HTTP] Request sucess. Code(%d)\n", httpCode);

    *response = this->http_client.getString();
    Log.trace("[HTTP] response[%d bytes] = %s\n", this->http_client.getSize(), response->c_str());
    this->http_client.end();

    return true;
  }
  Log.trace("[HTTP] Request failed. Code(%d): %s\n", httpCode, this->http_client.errorToString(httpCode).c_str());
  this->http_client.end();

  return false;
}
