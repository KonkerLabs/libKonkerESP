#include "json_helper.h"

JsonHelper::JsonHelper()
{
  memset(bufferChar, 0, 1024 * sizeof(char));
}

JsonHelper::~JsonHelper()
{
}

void JsonHelper::clearBuffer()
{
  memset(bufferChar, 0, 1024 * sizeof(char));
}

char * JsonHelper::createMessage(stringmap * content)
{
  StaticJsonDocument<512> jsonMSG;

  Log.trace("[JSON] Creating Json object\n");

  for(stringmap::iterator it = content->begin(); it!=content->end(); ++it)
  {
    // Log.trace("[JSON] INFO %s : %s\n", it->first.c_str(), it->second.c_str());
    jsonMSG[String(it->first.c_str())] = it->second.c_str();
  }

  clearBuffer();
  serializeJson(jsonMSG, bufferChar);

  return bufferChar;
}

bool JsonHelper::saveFile(const char * fileName, const char * buffer)
{
  if(LittleFS.begin())
  {
    Log.trace("[JSON] Saving %s file: %s\n", fileName, buffer);
    File fwFile = LittleFS.open(fileName, "w");
    if(fwFile)
    {
      fwFile.seek(0, SeekSet);
      int bytesWritten = fwFile.print(buffer);
      // int bytesWritten = fwFile.print(information);
      Log.trace("[JSON] Bytes written: %d\n", bytesWritten);
      fwFile.close();

      LittleFS.end();

      return true;
    }
    return false;
  }

  return false;
}

bool JsonHelper::saveCurrentFwInfo(const char * jsonBuffer)
{
  return saveFile("firmware_info.json", jsonBuffer);
}

bool JsonHelper::saveStatusInfo()
{
  char jsonBuffer[512];

  serializeJson(jsonHealth, jsonBuffer);

  return saveFile("status_info.json", jsonBuffer);
}

bool JsonHelper::readFile(const char * fileName, char * content)
{
  // char fileBuffer[256];

  if(LittleFS.begin())
  {
    Log.trace("[JSON] Recovering %s file\n", fileName);
    if(LittleFS.exists(fileName))
    {
      File fwFile = LittleFS.open(fileName, "r");
      if(fwFile)
      {
        fwFile.seek(0, SeekSet);
        fwFile.readBytes(content, fwFile.size());
        fwFile.close();
        LittleFS.end();

        // fileBuffer[sizeBytes] = '\0';
        Log.trace("[JSON] Recovered FW info: %s\n", content);
        return true;
      }
      return false;
    }
    return false;
  }

  return false;
}

bool JsonHelper::loadCurrentFwInfo(DynamicJsonDocument * fwInfo)
{
  char fileBuffer[256];

  if(readFile("firmware_info.json", fileBuffer))
  {
    DeserializationError err = deserializeJson(*fwInfo, fileBuffer);
    if (err)
    {
      Log.notice("[JSON] Failed to desirialize json document. Code = %s\n", err.c_str());
      return false;
    }
    return true;
  }

  return false;
}

bool JsonHelper::loadStatusInfo()
{
  char fileBuffer[256];

  if(readFile("status_info.json", fileBuffer))
  {
    DeserializationError err = deserializeJson(jsonHealth, String(fileBuffer));
    if (err)
    {
      Log.notice("[JSON] Failed to desirialize json document. Code = %s\n", err.c_str());
      return false;
    }
    return true;
  }

  return false;
}

// Add a json with status information to jsonHealth
bool JsonHelper::addInfoObject(int key, stringmap * content)
{
  Log.trace("[JSON] Adding %d stage key to JSON\n", key);

  JsonObject obj = this->jsonHealth.createNestedObject(String(key));

  if (obj.isNull())
  {
    Log.notice("[JSON] Failed to allocate memory for JSON object\n");
    return false;
  }

  for(stringmap::iterator it = content->begin(); it!=content->end(); ++it)
  {
    Log.trace("[JSON] INFO %s : %s\n", it->first.c_str(), it->second.c_str());
    obj[String(it->first.c_str())] = String(it->second.c_str());
  }

  Log.trace("[JSON] jsonHealth size[%d bytes] = %d\n", jsonHealth.memoryUsage(), jsonHealth.size());
  return true;
}

// bool JsonHelper::mergeInfo(int key, stringmap * content)
// {
//   Log.trace("[JSON] Adding %d stage key to JSON\n", key);

//   StaticJsonDocument<128> temp;
//   JsonObject obj = temp.createNestedObject(String(key));

//   if (obj.isNull())
//   {
//     Log.notice("[JSON] Failed to allocate memory for JSON object\n");
//     return false;
//   }

//   for(stringmap::iterator it = content->begin(); it!=content->end(); ++it)
//   {
//     Log.trace("[JSON] INFO %s : %s\n", it->first.c_str(), it->second.c_str());
//     obj[String(it->first.c_str())] = String(it->second.c_str());
//   }

//   for (auto kvp : temp.as<JsonObject>())
//   {
//     jsonHealth[kvp.key()] = kvp.value();
//   }

//   Log.trace("[JSON] jsonHealth size[%d bytes] = %d\n", jsonHealth.memoryUsage(), jsonHealth.size());
//   return true;
// }

void JsonHelper::getStatusCollected(char * buffer)
{
  serializeJson(jsonHealth, buffer, 512);
}

void JsonHelper::printStatus()
{
  Log.trace("[JSON] Device information[%d bytes]\n", jsonHealth.memoryUsage()); //, healthBuffer);
  printAddresses();
  for(JsonPair keys : jsonHealth.as<JsonObject>())
  {
    JsonObject element = keys.value().as<JsonObject>();
    Serial.print(keys.key().c_str());
    Serial.print("=> ");
    for(JsonPair info : element)
    {
      Serial.print(info.key().c_str());
      Serial.print(": ");
      Serial.print(info.value().as<char *>());
      Serial.print(" ");
    }
    Serial.print("\n");
    // Log.trace("[JSON] %d mem: %s, vcc: %s, rssi: %s\n", i, jsonHealth[(char)i]["mem"], jsonHealth[(char)i]["vcc"], jsonHealth[(char)i]["rssi"]);
  }
}

void JsonHelper::printAddresses()
{
  for(unsigned int i=0; i < jsonHealth.size(); i++)
  {
    Log.trace("[JSON] Address: %X %X: {%X, %X, %X}\n", 
                &jsonHealth, jsonHealth[(char)i], jsonHealth[(char)i]["vcc"], 
                jsonHealth[(char)i]["mem"], jsonHealth[(char)i]["rssi"]);
  }
}

void JsonHelper::clearInfo()
{
  jsonHealth.clear();
}

JsonHelper jsonHelper;
