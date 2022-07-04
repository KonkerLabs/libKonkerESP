#ifndef __CONNECTION_PROTOCOL_H__
#define __CONNECTION_PROTOCOL_H__

#include "base_protocol.h"
#include "globals.h"

class ConnectionProtocol: public BaseProtocol
{
protected:
  void unimplemented(String method);

public:
  ConnectionProtocol();
  ~ConnectionProtocol();
  void init();

  int send(const char * channel, String payload);
  int receive(String *payload);
  int request(String *retPayload, String endpoint);

  int connect();
  int disconnect();
  int checkConnection();

  bool protocolLoop();
};

#endif
