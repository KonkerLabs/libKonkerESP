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

void BaseProtocol::setDeviceId(String id)
{
  platformManager.setDeviceId(id);
}

void BaseProtocol::setPlatformCredentials(String userid, String password)
{
  platformManager.setPlatformCredentials(userid, password);
}

int BaseProtocol::savePlatformCredentials()
{
  return platformManager.savePlatformCredentials();
}

bool BaseProtocol::getPlatformCredentials(String * response, String id)
{
  *response = "";

  return false;
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

String BaseProtocol::getDeviceId()
{
  return platformManager.getDeviceId();
}

bool BaseProtocol::isCredentialSet()
{
  return platformManager.isCredentialSet();
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
