#ifndef _BASE_PROTOCOL_H
#define _BASE_PROTOCOL_H

#include "protocol.h"

class BaseProtocol: public Protocol {  
  protected: 
    String host; 
    int port; 
    // credentials used by the connection 
    const char* userid;
    const char* password;

  public:
    void init();
    int send(String channel, String payload);
    int receive(String *payload);

    void setConnection(String host, int port);
    String getHost();
    int getPort();
    void setCredentials(const char *userid, const char *password);

};

#endif