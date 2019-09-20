#ifndef __HTTP_PROTOCOL_H__
#define __HTTP_PROTOCOL_H__

#include "../globals.h"
#include "connection_protocol.h"
#include <ESP8266HTTPClient.h> 

class HTTPProtocol: public ConnectionProtocol {
  private: 
    HTTPClient client;

    long failedComm = 0;

    unsigned long _last_time_http_request = 0;
    unsigned long _millis_delay_per_http_request = 5000;

  protected:

    void buildHTTPSUBTopic(char const channel[], char *topic);
    void buildHTTPPUBTopic(char const channel[], char *topic);


  public:

    HTTPProtocol();
    ~HTTPProtocol();

    int send(const char * channel, String payload);
    int receive(String *payload);


    int connect();
    int disconnect();
    bool checkConnection();

};

#endif