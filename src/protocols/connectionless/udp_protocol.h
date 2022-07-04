#ifndef __UDP_PROTOCOL_H__
#define __UDP_PROTOCOL_H__

#include <WiFiUdp.h>
#include "connectionless_protocol.h"

class UDPProtocol: public ConnectionLessProtocol {
private:
  WiFiUDP Udp;
public:
  UDPProtocol();

  void init();
  int send(String channel, String payload);
  int receive(String *payload);
};

#endif
