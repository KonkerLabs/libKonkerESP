#ifndef __CONNECTION_PROTOCOL_H__
#define __CONNECTION_PROTOCOL_H__

#include <Arduino.h>
#include "protocol.h"
#include "base_protocol.h"

class ConnectionProtocol: public BaseProtocol {

  protected: 

    void unimplemented(String method);

  public:

    ~ConnectionProtocol();
    void init();

    int send(const char * channel, String payload);
    int receive(String *payload);

    int connect();
    int disconnect();
    bool checkConnection();
};

#endif