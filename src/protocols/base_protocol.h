#ifndef _BASE_PROTOCOL_H
#define _BASE_PROTOCOL_H

#include "protocol.h"
#include "globals.h"
#include "manage_platform.h"

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
  int request(String *retPayload, String endpoint);

  void setConnection(String host, int port);
  String getHost();
  int getPort();
  int getNumConnFail();
  void setNumConnFail(uint16_t count);
  void increaseConnFail();

  void setPlatformCredentials(String userid, String password);
  String getUser();
  String getPassword();
  bool isCredentialSet();
  void setCredentialStatus(bool status);

  int savePlatformCredentials();
  int restorePlatformCredentials();

  void setupClient(void *, String);

  int connect();
  int disconnect();
  int checkConnection();
  bool isConnectionOriented();

  bool protocolLoop();
};

#endif /* _BASE_PROTOCOL_H */
