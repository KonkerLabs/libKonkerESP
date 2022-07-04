#include "connection_protocol.h"

ConnectionProtocol::ConnectionProtocol() : BaseProtocol()
{
}

void ConnectionProtocol::init()
{
  BaseProtocol::init();
  // connect();
}

ConnectionProtocol::~ConnectionProtocol()
{
  if (checkConnection())
  {
    // disconnect and clear local variables
    disconnect();
  }
}

bool ConnectionProtocol::protocolLoop()
{
  return true;
}

void ConnectionProtocol::unimplemented(String method)
{
  Serial.println(method + " NOT IMPLEMENTED");
}


int ConnectionProtocol::send(const char * channel, String payload) {unimplemented("send"); return 0;}
int ConnectionProtocol::receive(String *payload) {unimplemented("receive"); return 0;}
int ConnectionProtocol::request(String *retPayload, String endpoint) {unimplemented("receive"); return 0;}
int ConnectionProtocol::connect() {unimplemented("connect"); return 0;}
int ConnectionProtocol::disconnect() {unimplemented("disconnect"); return 0;}
int ConnectionProtocol::checkConnection() {unimplemented("disconnect"); return 0;}
