#include "connection_protocol.h"

void ConnectionProtocol::init() {
  BaseProtocol::init();
  connect();
}

ConnectionProtocol::~ConnectionProtocol() {
  if (checkConnection()) {
    // disconnect and clear local variables
    disconnect();
  } 
}

void ConnectionProtocol::unimplemented(String method) {
  Serial.println(method + " NOT IMPLEMENTED");
}


int ConnectionProtocol::send(const char * channel, String payload) {unimplemented("send");}
int ConnectionProtocol::receive(String *payload) {unimplemented("receive");}
int ConnectionProtocol::connect() {unimplemented("connect");}
int ConnectionProtocol::disconnect() {unimplemented("disconnect");}
bool ConnectionProtocol::checkConnection() {unimplemented("checkConnection");}



