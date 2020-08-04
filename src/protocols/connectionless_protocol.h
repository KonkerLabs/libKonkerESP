#ifndef __CONNECTIONLESS_PROTOCOL_H__
#define __CONNECTIONLESS_PROTOCOL_H__

#include "base_protocol.h"

class ConnectionLessProtocol: public BaseProtocol
{
protected:
  void unimplemented(String method);

public:
  ConnectionLessProtocol();
  
  void init();
  int send(String channel, String payload);
  int receive(String *payload);
};

#endif
