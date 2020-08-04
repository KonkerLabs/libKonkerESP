#ifndef __JSON_H_H__
#define __JSON_H_H__

#include <string>
#include <unordered_map>
#include <ArduinoJson.h>
#include "globals.h"

class JsonHelper
{
private:
  char bufferChar[1024];

  void clearBuffer();
public:
  JsonHelper();
  ~JsonHelper();

  char * createMessage(stringmap * content);
  stringmap * parseJson(char  * content) {return nullptr;};
};

extern JsonHelper jsonHelper;

#endif /* __JSON_H_H__ */
