#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <Arduino.h>
#include <WiFiClient.h>

class Protocol
{
public:
  virtual void init() = 0;
  virtual int send(const char* , String) = 0;
  virtual int receive(String *) = 0;
  virtual int request(String *, String) = 0;

  virtual void setConnection(String, int);
  virtual String getHost();
  virtual int getPort();
  virtual int getNumConnFail();
  virtual void setNumConnFail(uint16_t);
  virtual void increaseConnFail();

  virtual void setPlatformCredentials(String, String);
  virtual String getUser();
  virtual String getPassword();
  virtual bool isCredentialSet();

  virtual int savePlatformCredentials();
  virtual int restorePlatformCredentials();
  virtual bool getPlatformCredentials(String *, String);

  virtual int connect();
  virtual int disconnect(); //return connection status from globals.h
  virtual int checkConnection();
  virtual bool isConnectionOriented();

  virtual bool protocolLoop();
};

#endif /* __PROTOCOL_H__ */
