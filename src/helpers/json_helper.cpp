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

JsonHelper jsonHelper;
