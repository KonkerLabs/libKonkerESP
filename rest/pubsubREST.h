#ifndef pubsubREST
#define pubsubREST

#include <iostream>

#include "../helpers/globals.h"
#ifndef ESP32
#include <ESP8266HTTPClient.h>
#else
#include <HTTPClient.h>
#endif

unsigned long _last_time_http_request=0;
unsigned long _millis_delay_per_http_request=5000;
void buildHTTPSUBTopic(char const channel[], char *topic){
  char bffPort[6];
//  itoa (_rootPort,bffPort,10);
  itoa(_httpPort, bffPort, 10);
  if (String(_httpDomain).indexOf("http://", 0)>0){
    strcpy (topic,_httpDomain);
    strcat (topic,":");
    strcat (topic,bffPort);
    strcat (topic,"/");
    strcat (topic,sub_dev_modifier);
    strcat(topic,"/");
    strcat (topic,device_login);
    strcat(topic,"/");
    strcat (topic,channel);
  }else{
    strcpy (topic,"http://");
    strcat (topic,_httpDomain);
    strcat (topic,":");
    strcat (topic,bffPort);
    strcat (topic,"/");
    strcat (topic,sub_dev_modifier);
    strcat(topic,"/");
    strcat (topic,device_login);
    strcat(topic,"/");
    strcat (topic,channel);
  }
}

void buildHTTPPUBTopic(char const channel[], char *topic){
  char bffPort[6];
//  itoa (_rootPort,bffPort,10);
  itoa(_httpPort, bffPort, 10);
  if (String(_httpDomain).indexOf("http://", 0)>0){
    strcpy (topic,_httpDomain);
    strcat (topic,":");
    strcat (topic,bffPort);
    strcat (topic,"/");
    strcat (topic,pub_dev_modifier);
    strcat(topic,"/");
    strcat (topic,device_login);
    strcat(topic,"/");
    strcat (topic,channel);
  }else{
    strcpy (topic,"http://");
    strcat (topic,_httpDomain);
    strcat (topic,":");
    strcat (topic,bffPort);
    strcat (topic,"/");
    strcat (topic,pub_dev_modifier);
    strcat(topic,"/");
    strcat (topic,device_login);
    strcat(topic,"/");
    strcat (topic,channel);
  }
}



bool testHTTPSubscribeConn(){
    //throtle this call
  if ((millis()-_last_time_http_request) < _millis_delay_per_http_request){
      delay((millis()-_last_time_http_request));
  }
  _last_time_http_request = millis();
  bool subCode=0;

  std::cout << "sending HTTP" << std::endl;

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(device_login, device_pass);

  char buffer[100];

  buildHTTPSUBTopic("test",buffer);

  std::cout << "trying to connect to " << buffer << std::endl;

  http.begin((String)buffer);

  int httpCode = http.GET();

  //Serial.println("Testing HTTP subscribe to: " + url_to_call + "; httpcode:" + String(httpCode));
  //Serial.print(">");

  subCode=interpretHTTPCode(httpCode);


  if (!subCode){
    Serial.println("test failed");
    Serial.println("");
    http.end();   //Close connection
    return 0;
  }else{
    Serial.println("sucess");
    Serial.println("");

    String strPayload = http.getString();
    Serial.println("strPayload=" + strPayload);
    http.end();   //Close connection
    return 1;
  }
}



bool pubHttp(char const channel[], char const msg[], int *ret_code){
  //throtle this call
  if ((millis()-_last_time_http_request) < _millis_delay_per_http_request){
      delay((millis()-_last_time_http_request));
  }
  _last_time_http_request = millis();

  bool pubCode=0;


  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.setAuthorization(device_login, device_pass);

  char buffer[100];

  buildHTTPPUBTopic(channel,buffer);

  http.setTimeout(10000);

  http.begin((String)buffer);

  std::cout << "(B) POST TO DATA" << std::endl;

  int httpCode=http.POST(String(msg));
  std::cout << "(E) POST TO DATA" << std::endl;
  //Serial.println("Publishing to " + String(topic) + "; Body: " + String(msg) + "; httpcode: " + String(httpCode));
  //Serial.print(">");
  http.end();   //Close connection
  std::cout << "(_) POST TO DATA" << std::endl;

  *ret_code = httpCode;

  pubCode=interpretHTTPCode(httpCode);


  if (!pubCode){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
    return 0;
  }else{
    Serial.println("sucess");
    Serial.println("");
    return 1;
  }

}

bool pubHttp(char const channel[], char const msg[]){
  int ret_code;
  return pubHttp(channel, msg, &ret_code);

}



bool subHttp(char const channel[],CHANNEL_CALLBACK_SIGNATURE){
  //throtle this call
  if ((millis()-_last_time_http_request) < _millis_delay_per_http_request){
      delay((millis()-_last_time_http_request));
  }
  _last_time_http_request = millis();

  bool subCode=0;


  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(device_login, device_pass);

  char buffer[100];

  buildHTTPSUBTopic(channel,buffer);

  http.begin((String)buffer);

  int httpCode = http.GET();



  Serial.print(">");

  subCode=interpretHTTPCode(httpCode);

  if (!subCode){
    Serial.println("failed SUB request");
    Serial.println("");
    failedComm=1;
  }else{
    Serial.println("sucess SUB request");
    Serial.println("");

    String strPayload = http.getString();
    //Serial.println("strPayload=" + strPayload);
    int playloadSize=http.getSize();
    http.end();   //Close connection
    if (strPayload!="[]"){
      unsigned char* payload = (unsigned char*) strPayload.c_str(); // cast from string to unsigned char*
      Serial.println(strPayload);
      Serial.println("calling callback");
      chan_callback(payload,playloadSize);
      Serial.println("callback called");
    }
    return 1;
  }
  http.end();   //Close connection
  return 0;
}


#endif
