#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <Arduino.h>
#include <ESP8266HTTPClient.h>

class Protocol
{
public:
  virtual void init() = 0;
  virtual int send(const char* channel, String payload) = 0;
  virtual int receive(String *payload) = 0;
  virtual int request(String *retPayload, String endpoint) = 0;

  virtual void setConnection(String host, int port);
  virtual String getHost();
  virtual int getPort();
  virtual int getNumConnFail();
  virtual void setNumConnFail(uint16_t count);
  virtual void increaseConnFail();
  virtual void setupClient(void *, String);

  virtual void setPlatformCredentials(String userid, String password);
  virtual String getUser();
  virtual String getPassword();
  virtual bool isCredentialSet();
  virtual void setCredentialStatus(bool status);
  virtual int savePlatformCredentials();
  virtual int restorePlatformCredentials();

  virtual int connect();
  virtual int disconnect(); //return connection status from globals.h
  virtual int checkConnection();
  virtual bool isConnectionOriented();

  virtual bool protocolLoop();
};

#endif /* __PROTOCOL_H__ */
