#include "base_protocol.h"

BaseProtocol::BaseProtocol()
{
}

void BaseProtocol::init()
{
}

void BaseProtocol::protocolLoop()
{
  delay(1000);
}

int BaseProtocol::send(String channel, String payload)
{
  return 0;
}

int BaseProtocol::receive(String *payload)
{
  return 0;
}

void BaseProtocol::setConnection(String host, int port)
{
  platformManager.setServer(host, port);
}

String BaseProtocol::getHost()
{
  return platformManager.getHost();
}

int BaseProtocol::getPort()
{
  return platformManager.getPort();
}

void BaseProtocol::getClient(HTTPClient * http)
{
  http = nullptr;
}

void BaseProtocol::setPlatformCredentials(String userid, String password)
{
  platformManager.setPlatformCredentials(userid, password);
}

int BaseProtocol::savePlatformCredentials()
{
  return platformManager.savePlatformCredentials();
}

int BaseProtocol::restorePlatformCredentials()
{
  return platformManager.restorePlatformCredentials();
}

String BaseProtocol::getUser()
{
  return platformManager.getUser();
}

String BaseProtocol::getPassword()
{
  return platformManager.getPassword();
}

int BaseProtocol::connect()
{
  return 0;
}

int BaseProtocol::disconnect()
{
  return 0;
}

int BaseProtocol::checkConnection()
{
  return UNKNOWN;
}
