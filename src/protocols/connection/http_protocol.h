#ifndef __HTTP_PROTOCOL_H__
#define __HTTP_PROTOCOL_H__

#include <ESP8266HTTPClient.h>
#include "connection_protocol.h"

#define SUB_PREFIX "sub"
#define PUB_PREFIX "pub"

class HTTPProtocol: public ConnectionProtocol
{
private:
  HTTPClient http_client;
  WiFiClient wifi_client;

  unsigned long _last_time_http_request = 0;
  // unsigned long _millis_delay_per_http_request = 5000;

  String registry = "registry-data/";

public:

  HTTPProtocol();
  ~HTTPProtocol();

  int send(const char * channel, String payload);
  int receive(String *payload);
  int request(String *retPayload, String endpoint);

  int connect() override;
  int disconnect() override;
  int checkConnection() override;
  bool getPlatformCredentials(String *, String) override;

  void setRegistry(String reg);
};

#endif
