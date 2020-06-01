#include "konker.h"

const char ssid[20] = "dlink-C21E-114";
const char pwd[20] = "bobesponja";

// Dados do servidor
String server_ip = "mqtt.demo.konkerlabs.net";
int mqtt_port = 1883;
// int http_port = 8082;
// int fw_port = 8081;

String DEV_ID = "node10";
String USER = "pgdmna95n2o2";
String PWD = "HnWDYsNGdlcb";

KonkerDevice device;
bool connected;

void setup()
{
  // put your setup code here, to run once:
  // Serial.begin(115200);
  Serial.println("\nStarting setup!");
  device.addWifi(ssid, pwd);
  device.setServer(server_ip, mqtt_port);
  device.setPlatformCredentials(DEV_ID, USER, PWD);
  device.setDefaultConnectionType(ConnectionType::MQTT);

  device.connectWifi();
  device.startConnection();
}

void loop()
{
  // put your main code here, to run repeatedly
  connected = device.checkWifiConnection();
  Serial.println("WiFi is " + String(connected));

  connected = device.checkPlatformConnection();

  // device.loop();
  delay(1000);
}
