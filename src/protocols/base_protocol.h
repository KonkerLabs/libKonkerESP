#ifndef _BASE_PROTOCOL_H
#define _BASE_PROTOCOL_H

#include "protocol.h"
#include "globals.h"

class BaseProtocol: public Protocol
{
protected:
  String host;
  int port;
  // credentials used by the connection
  const char* userid;
  const char* password;

public:
  BaseProtocol();
  void init();
  int send(String channel, String payload);
  int receive(String *payload);

  void setConnection(String host, int port);
  void setCredentials(const char *userid, const char *password);
  String getHost();
  int getPort();
  void getClient(HTTPClient *);

  int connect();
  int disconnect();
  int checkConnection();

  void protocolLoop();
};

#endif
