#include "base_protocol.h"

BaseProtocol::BaseProtocol()
{
  this->connectionOriented = false;
}

void BaseProtocol::init()
{
}

bool BaseProtocol::protocolLoop()
{
  delay(1000); //1s
  if(this->numConnFail > MAX_CONN_FAIL)
    return true;
  return false;
}

int BaseProtocol::send(String channel, String payload)
{
  return 0;
}

int BaseProtocol::receive(String *payload)
{
  return 0;
}

int BaseProtocol::request(String * retPayload, String endpoint)
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

int BaseProtocol::getNumConnFail()
{
  return this->numConnFail;
}

void BaseProtocol::setNumConnFail(uint16_t count)
{
  this->numConnFail = count;
}

void BaseProtocol::increaseConnFail()
{
  this->numConnFail++;
}

void BaseProtocol::getClient(void * http)
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

bool BaseProtocol::isCredentialSet()
{
  return platformManager.isCredentialSet();
}

void BaseProtocol::setCredentialStatus(bool status)
{
  platformManager.setCredentialStatus(status);
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

bool BaseProtocol::isConnectionOriented()
{
  return connectionOriented;
}
