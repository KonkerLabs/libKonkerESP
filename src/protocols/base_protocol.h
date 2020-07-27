#ifndef _BASE_PROTOCOL_H
#define _BASE_PROTOCOL_H

#include "protocol.h"
#include "manage_platform.h"
#include "globals.h"

class BaseProtocol: public Protocol
{
protected:
  PlatformManager platformManager;
  bool connectionOriented;
  int numConnFail = 0;

  friend class HealthMonitor;

public:
  BaseProtocol();
  void init();
  int send(String channel, String payload);
  int receive(String *payload);

  void setConnection(String host, int port);
  String getHost();
  int getPort();
  int getNumConnFail();
  void setPlatformCredentials(String userid, String password);
  String getUser();
  String getPassword();

  bool isCredentialSet();
  int savePlatformCredentials();
  int restorePlatformCredentials();

  void getClient(HTTPClient *); // [MJ] Change this to use void*

  int connect();
  int disconnect();
  int checkConnection();
  bool isConnectionOriented();

  void protocolLoop();
};

#endif /* _BASE_PROTOCOL_H */
