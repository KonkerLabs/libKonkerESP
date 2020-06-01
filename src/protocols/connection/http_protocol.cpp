#include "http_protocol.h"

#define SUB_PREFIX "sub"
#define PUB_PREFIX "pub"

HTTPProtocol::HTTPProtocol() : http_client()
{
}

HTTPProtocol::~HTTPProtocol()
{
  this->disconnect();
}

void HTTPProtocol::getClient(HTTPClient* http)
{
  http = &this->http_client;
}

int HTTPProtocol::connect()
{
  return this->http_client.begin(wifi_client, "http://"+this->host+":"+this->port);
}

int HTTPProtocol::disconnect()
{
  if (this->http_client.connected())
  {
    this->http_client.end();
  }
  return DISCONNECTED;
}
int HTTPProtocol::checkConnection()
{
  return this->http_client.connected();
}

void HTTPProtocol::buildHTTPSUBTopic(char const channel[], char *topic){
  char bffPort[6];
//  itoa (_rootPort,bffPort,10);
  itoa(port, bffPort, 10);
  strcpy (topic,"http://");
  strcat (topic,host.c_str());
  strcat (topic,":");
  strcat (topic,bffPort);
  strcat (topic,"/");
  strcat (topic,SUB_PREFIX);
  strcat(topic,"/");
  strcat (topic,this->userid);
  strcat(topic,"/");
  strcat (topic,channel);
}

void HTTPProtocol::buildHTTPPUBTopic(char const channel[], char *topic){
  char bffPort[6];
//  itoa (_rootPort,bffPort,10);
  itoa(port, bffPort, 10);
  strcpy (topic,"http://");
  strcat (topic,host.c_str());
  strcat (topic,":");
  strcat (topic,bffPort);
  strcat (topic,"/");
  strcat (topic,PUB_PREFIX);
  strcat(topic,"/");
  strcat (topic,this->userid);
//  itoa (_rootPort,bffPort,10);
  itoa(port, bffPort, 10);
  strcpy (topic,"http://");
  strcat (topic,host.c_str());
  strcat (topic,":");
  strcat (topic,bffPort);
  strcat (topic,"/");
  strcat (topic,PUB_PREFIX);
  strcat(topic,"/");
  strcat (topic,this->userid);
  strcat(topic,"/");
  strcat (topic,channel);
}

int HTTPProtocol::send(const char *channel, String payload) {

  //throtle this call
  if ((millis()-_last_time_http_request) < _millis_delay_per_http_request){
      delay((millis()-_last_time_http_request));
  }
  _last_time_http_request = millis();

  http_client.addHeader("Content-Type", "application/json");
  http_client.addHeader("Accept", "application/json");
  http_client.setAuthorization(userid, password);

  char buffer[100];

  buildHTTPPUBTopic(channel,buffer);

  http_client.setTimeout(10000);

  // std::cout << "(B) POST TO DATA" << std::endl;

  int httpCode=http_client.POST(payload);
  // std::cout << "(E) POST TO DATA" << std::endl;

  if (httpCode >= 0 && httpCode < 300) {
    // std::cout << "success " << httpCode << std::endl;
    return 1;
  } else {
    // std::cout << "failed " << httpCode << std::endl;
    failedComm=1;
    return 0;
  }
}

int HTTPProtocol::receive(String *payload)
{
  return 0;
}
