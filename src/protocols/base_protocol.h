#ifndef _BASE_PROTOCOL_H
#define _BASE_PROTOCOL_H

#include "protocol.h"
#include "manage_platform.h"
#include "globals.h"

class BaseProtocol: public Protocol
{
protected:
  PlatformManager platformManager;

public:
  BaseProtocol();
  void init();
  int send(String channel, String payload);
  int receive(String *payload);

  void setConnection(String host, int port);
  String getHost();
  int getPort();
  void setPlatformCredentials(String userid, String password);
  String getUser();
  String getPassword();

  int savePlatformCredentials();
  int restorePlatformCredentials();

  void getClient(HTTPClient *); // [MJ] Change this to use void*

  int connect();
  int disconnect();
  int checkConnection();

  void protocolLoop();
};

#endif
