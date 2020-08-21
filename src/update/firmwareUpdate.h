/*
  Firmware update class. Part of Konker FW.
  @author Maria Júlia Berriel de Sousa
*/
#ifndef _FW_UPDATE_H_
#define _FW_UPDATE_H_

#include <ESP8266httpUpdate.h>
#include <WiFiClient.h>
#include "globals.h"
#include "manifest_handler.h"
#include "../protocols/connection/http_protocol.h"

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

// Usar?
// void onStart(HTTPUpdateStartCB cbOnStart)          { _cbStart = cbOnStart; }
// void onEnd(HTTPUpdateEndCB cbOnEnd)                { _cbEnd = cbOnEnd; }
// void onError(HTTPUpdateErrorCB cbOnError)          { _cbError = cbOnError; }
// void onProgress(HTTPUpdateProgressCB cbOnProgress) { _cbProgress = cbOnProgress; }
// using HTTPUpdateStartCB = std::function<void()>;
// using HTTPUpdateEndCB = std::function<void()>;
// using HTTPUpdateErrorCB = std::function<void(int)>;
// using HTTPUpdateProgressCB = std::function<void(int, int)>;

namespace updateGlobals
{
  static Protocol * httpProtocolGlobal = nullptr;
  static String * manifestEndpointGlobal = nullptr;
}
void atUpdateStart();
void atUpdateProgress(int, int);
void atUpdateEnd();

class ESPHTTPKonkerUpdate: public ESP8266HTTPUpdate
{
private:
  Protocol * _httpProtocol;
  String _manifestEndpoint;
  String _fwEndpoint;
  State _deviceState;
  unsigned long _last_time_update_check;

  fw_info_t currentFwInfo;
  ManifestHandler * manifest;

  // returns true if there is a new manifest
  int querryPlatform();
  bool validateUpdate();

  t_httpUpdate_return update(String newVersion);
  bool addtionalSteps(bool correct);

  void sendStatusMessage(int msgIndex);
  void sendUpdateConfirmation();
  void sendExceptionMessage(int exptIndex);

public:
  ESPHTTPKonkerUpdate(Protocol *client, String * manifestEndpoint);
  ESPHTTPKonkerUpdate();
  ~ESPHTTPKonkerUpdate();

  void setProtocol(Protocol *client);
  // void setFWchannel(String id);

  bool checkForUpdate();
  void performUpdate();
  //To be called during setup
  bool checkFirstBoot();

  static void sendFwReceivedMessage();
};

#endif /* _FW_UPDATE_H_ */
