#include "konker.h"

const char ssid[10] = "yellow";
const char pwd[30] = "!yellow2016";

// Dados do servidor 
char server_ip[50] = "mqtt.demo.konkerlabs.net";
char mqtt_port[8] = "1883";
int http_port = 8082;
int fw_port = 8081;

char DEV_ID[10] = "node10";
char USER[16] = "i722cp5hgil5"; //03local"sqsvldcfdmlg"; //02local:"b2evd357tmgl"; //02web: "j761nvqo5qoq";
char PWD[16] = "nH4rPzJvmKSN";

KonkerDevice device;

void setup() {
  // put your setup code here, to run once:
  device.addWifi(ssid, pwd);
  device.setPlatformCredentials(USER, PWD);
}

void loop() {
  // put your main code here, to run repeatedly:
  device.loop();
}
