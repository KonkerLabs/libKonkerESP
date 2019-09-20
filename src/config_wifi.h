#ifndef __CONFIG_WIFI_H__
#define __CONFIG_WIFI_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>

class ConfigWifi{
    private:
    bool i=0;
    bool g=0;
    bool s=0;

    public:
    IPAddress ip;   
    IPAddress gateway;   
    IPAddress subnet;  

    ConfigWifi(); 
    void setIP (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    void setGateway (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    void setSubnet (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    bool isConfigured();

};

#endif