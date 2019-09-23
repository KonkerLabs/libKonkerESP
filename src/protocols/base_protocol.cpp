#include "base_protocol.h"

void BaseProtocol::init() {
}

int BaseProtocol::send(String channel, String payload) {
    return 0;
}

int BaseProtocol::receive(String *payload) {
    return 0;
}

void BaseProtocol::setConnection(String host, int port) {
  this->host = host;
  this->port = port;
}

String BaseProtocol::getHost() {
  return this->host;
}

int BaseProtocol::getPort() {
  return this->port;
}

void BaseProtocol::setCredentials(const char *userid, const char *password) {
  this->userid = userid;
  this->password = password;
}
