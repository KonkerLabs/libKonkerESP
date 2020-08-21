#include "konker.h"
ADC_MODE(ADC_VCC);

const char ssid[20] = "dlink-C21E-114";
const char pwd[20] = "bobesponja";

// Dados do servidor
String server_ip = "192.168.0.123";
int mqtt_port = 32768;
// String server_ip = "mqtt.prod.konkerlabs.net";
// int mqtt_port = 1883;
int http_port = 8082;
// int fw_port = 8081;

String DEV_ID = "node02";
String USER = "b2evd357tmgl";
String PWD = "WpyGjpYKaDRS";
// String USER = "j761nvqo5qoq"; //"pgdmna95n2o2"; //"t97pvjblbeas";
// String PWD = "8FVki75P8AAy"; //"HnWDYsNGdlcb"; //"YJ2GskQvqU8S";

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
  Serial.println("\nStarting setup!");
  Serial.println("====== Saving credentials ======");
  device.addWifi(ssid, pwd);
  device.setDefaultConnectionType(ConnectionType::MQTT);
  // device.setServer(server_ip, mqtt_port);
  device.setServer(server_ip, mqtt_port, http_port);
  device.setPlatformCredentials(DEV_ID, USER, PWD);

  Serial.println("====== Connecting ======");
  device.connectWifi();
  // start platform connection
  device.startConnection(false);

  device.saveAllCredentials();
  Serial.println("====== Setup finished ======");
  // Serial.println("====== NEW VERSION ======");

  lasttimeSend = millis();
}

void loop()
{
  unsigned int loop_duration = micros();
  count = count + 1;
  connected = device.checkWifiConnection();
  if (!connected) device.connectWifi();

  if(count % 4 == 0)
  {
    mensagem = jsonMQTTmsgDATA(DEV_ID.c_str(), "Celsius", count);
    device.storeData(PUB, mensagem);
  }

  connected = device.checkPlatformConnection();
  if(!connected)
  {
    Serial.println("Disconnected from platform! Reconnecting...");
    device.startConnection(true);
  }
  // Serial.println("Connection to platform " + String(connected));

  // TODO mover para device.loop()
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

    // char ts[20];
    // device.getCurrentTime(ts);
    // Serial.println("TIME TIME TIME " + String(ts));
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
  device.loopDuration(micros() - loop_duration);
}
