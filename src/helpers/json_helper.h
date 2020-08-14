#ifndef __JSON_H_H__
#define __JSON_H_H__

#include <string>
#include <unordered_map>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include "globals.h"

typedef std::unordered_map<std::string, std::string> stringmap;

class JsonHelper
{
private:
  char bufferChar[1024];

  void clearBuffer();
public:
  JsonHelper();
  ~JsonHelper();

  char * createMessage(stringmap * content);
  // stringmap * parseJson(char  * content) {return nullptr;};

  bool saveCurrentFwInfo();
  bool loadCurrentFwInfo(DynamicJsonDocument * fwInfo);
};

extern JsonHelper jsonHelper;

#endif /* __JSON_H_H__ */
