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

  virtual void setConnection(String host, int port);
  virtual String getHost();
  virtual int getPort();
  virtual void getClient(HTTPClient *);

  virtual void setPlatformCredentials(String userid, String password);

  virtual int savePlatformCredentials();
  virtual int restorePlatformCredentials();

  virtual int connect();
  virtual int disconnect(); //return connection status from globals.h
  virtual int checkConnection(); //returning integer meaning depends on protocol library

  virtual void protocolLoop();
};

#endif
