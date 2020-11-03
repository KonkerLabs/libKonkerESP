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

bool JsonHelper::saveCurrentFwInfo(const char * jsonBuffer)
{
  if(LittleFS.begin())
  {
    Log.trace("[JSON] Saving current FW info: %s\n", jsonBuffer);
    File fwFile = LittleFS.open("firmware_info.json", "w");
    if(fwFile)
    {
      fwFile.seek(0, SeekSet);
      int bytesWritten = fwFile.print(jsonBuffer);
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

bool JsonHelper::loadCurrentFwInfo(DynamicJsonDocument * fwInfo)
{
  char fileBuffer[256];

  if(LittleFS.begin())
  {
    Log.trace("[JSON] Recovering current FW info\n");
    if(LittleFS.exists("firmware_info.json"))
    {
      File fwFile = LittleFS.open("firmware_info.json", "r");
      if(fwFile)
      {
        fwFile.seek(0, SeekSet);
        fwFile.readBytes(fileBuffer, fwFile.size());
        fwFile.close();
        LittleFS.end();

        // fileBuffer[sizeBytes] = '\0';
        Log.trace("[JSON] Recovered FW info: %s\n", fileBuffer);
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
    return false;
  }

  return false;
}

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
    obj[String(it->first.c_str())] = it->second.c_str();
  }

  Log.trace("[JSON] jsonHealth size[%d bytes] = %d\n", jsonHealth.memoryUsage(), jsonHealth.size());
  return true;
}

void JsonHelper::printStatus()
{
  char healthBuffer[512];

  serializeJson(jsonHealth, healthBuffer);
  Log.trace("[JSON] Device information[%d bytes]: %s\n", jsonHealth.memoryUsage(), healthBuffer);
  for(int i=0; i < jsonHealth.size(); i++)
  {
    Log.trace("[JSON] Address: 0x%X 0x%X: {0x%X, 0x%X, 0x%X}\n", 
                &jsonHealth, &jsonHealth[(char)i], &jsonHealth[(char)i]["vcc"], 
                &jsonHealth[(char)i]["mem"], &jsonHealth[(char)i]["rssi"]);
  }
}

void JsonHelper::printAddresses()
{
  for(int i=0; i < jsonHealth.size(); i++)
  {
    Log.trace("[JSON] Address: 0x%X 0x%X: {0x%X, 0x%X, 0x%X}\n", 
                &jsonHealth, &jsonHealth[(char)i], &jsonHealth[(char)i]["vcc"], 
                &jsonHealth[(char)i]["mem"], &jsonHealth[(char)i]["rssi"]);
  }
}

void JsonHelper::clearInfo()
{
  jsonHealth.clear();
}

JsonHelper jsonHelper;
