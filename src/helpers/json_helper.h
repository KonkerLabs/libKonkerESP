#ifndef __JSON_H_H__
#define __JSON_H_H__

#include <unordered_map>
#include <string>
#include <ArduinoJson.h>
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
};

extern JsonHelper jsonHelper;

#endif /* __JSON_H_H__ */
