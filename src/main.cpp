#include "konker.h"
ADC_MODE(ADC_VCC);

const char ssid[20] = "dlink-C21E-114";
const char pwd[20] = "bobesponja";
const char ssid2[20] = "maju_iot";
const char pwd2[20] = "unicamp123";

// Dados do servidor
String server_ip = "192.168.0.123";
// String server_ip = "mqtt.prod.konkerlabs.net";
int mqtt_port = 32768;
// int mqtt_port = 1883;
int http_port = 8082;

// String DEV_ID = "node10";
// String USER = "t97pvjblbeas";
// String PWD = "YJ2GskQvqU8S";
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
  const int capacity = 1024;
  StaticJsonDocument<capacity> jsonMSG;
  char ts[20];

  device.getCurrentTime(ts);
  jsonMSG["deviceId"] = device_id;
  // jsonMSG["metric"] = metric;
  jsonMSG["value"] = value;
  jsonMSG["ts_queued"] = ts;
  serializeJson(jsonMSG, bufferJson);

  Serial.print("Mensagem >> ");
  Serial.println(bufferJson);

  return bufferJson;
}

void setup()
{
  Serial.println("\nStarting setup!");
  Serial.println("====== Setting up credentials ======");
  device.addWifi(ssid2, pwd2);
  device.addWifi(ssid, pwd);
  device.setDefaultConnectionType(ConnectionType::MQTT);
  // device.setServer(server_ip, mqtt_port);
  device.setServer(server_ip, mqtt_port, http_port);
  // device.setPlatformCredentials(DEV_ID, USER, PWD);

  Serial.println("====== Connecting ======");
  // start wifi and platform connection
  device.init();

  // device.saveAllCredentials();
  Serial.println("====== Setup finished ======");
  // Serial.println("====== NEW VERSION = =====");

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
    mensagem = jsonMQTTmsgDATA(device.getDeviceId().c_str(), "Celsius", count);
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
  if((connected) && ((millis() - lasttimeSend) > 5000))
  {
    int ok = device.sendData();
    if (ok)
    {
      Serial.println("YAAAAY");
      lasttimeSend = millis();
    }
    else
    {
      Serial.println("NOPE");
    }
  }

  device.loop();
  device.loopDuration(micros() - loop_duration);
}
