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
  void setNumConnFail(uint16_t count);
  void increaseConnFail();

  void setPlatformCredentials(String userid, String password);
  bool isCredentialSet();
  String getUser();
  String getPassword();

  int savePlatformCredentials();
  int restorePlatformCredentials();

  void getClient(HTTPClient *); // [TODO] Change this to use void*

  int connect();
  int disconnect();
  int checkConnection();
  bool isConnectionOriented();

  bool protocolLoop();
};

#endif /* _BASE_PROTOCOL_H */
