#ifndef _FW_DEFINITONS_H_
#define _FW_DEFINITONS_H_

#include <WiFiClient.h>
#include <ESP8266httpUpdate.h>
#include "globals.h"
#include "manifest_handler.h"
#include "manage_eeprom.h"
#include "health_monitor.h"
#include "../protocols/connection/http_protocol.h"

#define FIRST_BOOT_FLAG 0xFFu

enum State {RUNNING, UPDATING, REBOOT, UPDATED};

enum StatusMessages
{
  MSG_MANIFEST_RECEIVED = 0,
  MSG_MANIFEST_CORRECT,
  MSG_FIRMWARE_RECEIVED,
  MSG_CHECKSUM_OK,
  MSG_UPDATE_DONE,
  MSG_UPDATE_CORRECT
};

enum ExceptionMessages
{
  EXPT_COULD_NOT_GET_MAN = 0,
  EXPT_MANIFEST_INCORRECT,
  EXPT_FW_NOT_FOUND,
  EXPT_CHECKSUM_NOK,
  EXPT_INCORRECT_START
};

namespace updateGlobals
{
  extern Protocol * httpProtocolGlobal;
  extern String * manifestEndpointGlobal;
}

// void onStart(HTTPUpdateStartCB cbOnStart)          { _cbStart = cbOnStart; }
// void onEnd(HTTPUpdateEndCB cbOnEnd)                { _cbEnd = cbOnEnd; }
// void onError(HTTPUpdateErrorCB cbOnError)          { _cbError = cbOnError; }
// void onProgress(HTTPUpdateProgressCB cbOnProgress) { _cbProgress = cbOnProgress; }
// using HTTPUpdateStartCB = std::function<void()>;
// using HTTPUpdateEndCB = std::function<void()>;
// using HTTPUpdateErrorCB = std::function<void(int)>;
// using HTTPUpdateProgressCB = std::function<void(int, int)>;

void atUpdateStart();
void atUpdateProgress(int, int);
void atUpdateEnd();

#endif /* _FW_DEFINITONS_H_ */
