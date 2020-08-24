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
  // [MJ] for testing
  // const char * jsonBuffer = "{\"version\":\"1.0.0\", \"sequence_number\":\"3796729686678\", \"device\":\"node02\"}";

  // fw_info_t information;
  // strcpy(information.version, "1.0.0");
  // strcpy(information.deviceID, "node02");
  // information.seqNumber = (unsigned long)3796729686678;

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

        // fileBuffer[sizeBytes] = '\0';
        Log.trace("[JSON] Recovered FW info: %s\n", fileBuffer);
        deserializeJson(*fwInfo, fileBuffer);
        LittleFS.end();

        return true;
      }
      return false;
    }
    return false;
  }

  return false;
}

JsonHelper jsonHelper;
