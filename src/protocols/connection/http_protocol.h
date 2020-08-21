#ifndef __HTTP_PROTOCOL_H__
#define __HTTP_PROTOCOL_H__

#include "connection_protocol.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

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

  void setupClient(void * http, String uri);

  int send(const char * channel, String payload);
  int receive(String *payload);
  int request(String *retPayload, String endpoint);

  int connect() override;
  int disconnect() override;
  int checkConnection() override;

  void setRegistry(String reg);
};

#endif
