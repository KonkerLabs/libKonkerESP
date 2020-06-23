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

String PUB = "temp";

KonkerDevice device;
bool connected;
char bufferJson[256];
char * mensagem;
BufferElement data;

int count = 0;
long lasttimeSend=0;

char *jsonMQTTmsgDATA(const char *device_id, const char *metric, long value)
{
  const int capacity = 1024; // JSON_OBJECT_SIZE(200);
  StaticJsonDocument<capacity> jsonMSG;

  jsonMSG["deviceId"] = device_id;
  jsonMSG["metric"] = metric;
  jsonMSG["value"] = value;
  serializeJson(jsonMSG, bufferJson);

  Serial.print("Mensagem >> ");
  Serial.println(bufferJson);

  return bufferJson;
}

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
  // start platform connection
  device.startConnection();

  lasttimeSend = millis();
}

void loop()
{
  count = count + 1;
  // put your main code here, to run repeatedly
  connected = device.checkWifiConnection();
  // Serial.println("WiFi is " + String(connected));

  if(count % 4 == 0)
  {
    mensagem = jsonMQTTmsgDATA(DEV_ID.c_str(), "Celsius", count);
    device.storeData(PUB, mensagem);
  }

  connected = device.checkPlatformConnection();
  if(!connected)
  {
    Serial.println("Disconnected from platform! Reconnecting...");
    device.startConnection();
  }
  // Serial.println("Connection to platform " + String(connected));

  if((connected) && ((millis() - lasttimeSend) > 10000))
  {
    int ok = device.sendData();
    if (ok)
    {
      Serial.println("YAAAAY");
    }
    else
    {
      Serial.println("NOPE");
    }

    ok = device.testSendHTTP();
    if (ok)
    {
      Serial.println("Send via HTTP working!");
    }
    else
    {
      Serial.println("Did not send via HTTP :(");
    }
  }

  // if((millis() - lasttimeSend) > 5000) //ms
  // {
  //   mensagem = jsonMQTTmsgDATA(DEV_ID.c_str(), "Celsius", count);
  //   device.sendData(PUB, String(mensagem));
  //   data = device.recoverData();
  //   Serial.print("Removed from buffer >>> ");
  //   Serial.print(data.payload);
  //   Serial.print(" | ");
  //   Serial.println(data.channel);
  //   lasttimeSend = millis();
  // }

  device.loop();
}
